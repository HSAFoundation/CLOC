typedef struct {
    uint M;
    uint N;
    uint K;
    float alpha;
    float beta;
    uint A;
    uint B;
    uint C;
    uint lda;
    uint ldb;
    uint ldc;
    uint offA;
    uint offB;
    uint offC;
} gemmArgs;

uint gemmOffsetInit = 0;

uint gemm_offA[10] = {256,   384,   512,   384,    82304,  512,    82432,  512,    82432,  164352};
uint gemm_offB[10] = {128,   128,   128,   256,    82176,  256,    82176,  384,    82304,  164224};
uint gemm_offC[10] = {82176, 82304, 82432, 164224, 164224, 164352, 164352, 246272, 246272, 246272};

void computeGemmOffset( int n, int nb ) {
    int j;
    int currentInvocationID = 0;
    for ( j = 0; j < n; j += nb ) {
        assert( nb <= n-j );
        if( j+nb < n && j > 0) {
            assert(currentInvocationID < 10);
            int tile_row;
            int tile_col = j;
            int col_iter;
            for (tile_row = j + nb; tile_row < n; tile_row += nb) {
                for (col_iter = 0; col_iter < tile_col; col_iter += nb) {
                    int offA = tile_row + col_iter * n;
                    int offB = j + col_iter * n;
                    int offC = tile_row + j * n;

                    //// To be removed later
                    //assert(gemm_offA[currentInvocationID] == offA);
                    //assert(gemm_offB[currentInvocationID] == offB);
                    //assert(gemm_offC[currentInvocationID] == offC);

                    // Assign the computed offsets
                    gemm_offA[currentInvocationID] = offA;
                    gemm_offB[currentInvocationID] = offB;
                    gemm_offC[currentInvocationID] = offC;

                    currentInvocationID++;
                }
            }
        }
    }
}

void initGemmArgs(gemmArgs* thisGemmArgs, int n, int nb, int invocationID) {
    if (gemmOffsetInit == 0) {
        computeGemmOffset(n, nb);
        gemmOffsetInit = 1;
    }
    thisGemmArgs->M = nb;
    thisGemmArgs->N = nb;
    thisGemmArgs->K = nb;
    thisGemmArgs->alpha = -1.0;
    thisGemmArgs->beta = 1.0;
    thisGemmArgs->A = 0;
    thisGemmArgs->B = 0;
    thisGemmArgs->C = 0;
    thisGemmArgs->lda = n;
    thisGemmArgs->ldb = n;
    thisGemmArgs->ldc = n;
    thisGemmArgs->offA = gemm_offA[invocationID];
    thisGemmArgs->offB = gemm_offB[invocationID];
    thisGemmArgs->offC = gemm_offC[invocationID];
}
