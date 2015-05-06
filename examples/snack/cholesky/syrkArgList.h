typedef struct {
    uint N;
    uint K;
    float alpha;
    uint A;
    uint lda;
    float beta;
    uint C;
    uint ldc;
    uint startN;
    uint origN;
    uint offA;
    uint offB;
    uint offC;
} syrkArgs;

uint syrkOffsetInit = 0;

uint syrk_offA[10] = {128,   256,    82176,  384,    82304,  164224, 512,    82432,  164352, 246272};
uint syrk_offB[10] = {128,   256,    82176,  384,    82304,  164224, 512,    82432,  164352, 246272};
uint syrk_offC[10] = {82048, 164096, 164096, 246144, 246144, 246144, 328192, 328192, 328192, 328192};

void computeSyrkOffset(int n, int nb) {
    int j;
    int currentInvocationID = 0;
    for ( j = 0; j < n; j += nb ) {
        assert( nb <= n-j );
        if( j>0 ) {
            assert(currentInvocationID < 10);
            int tile_row = j;
            int tile_col = j;
            int col_iter;
            for (col_iter = 0; col_iter < tile_col; col_iter += nb) {
                int offA = j + col_iter * n; 
                int offB = j + col_iter * n; 
                int offC = j + j * n; 

                //// To be removed later
                //assert(syrk_offA[currentInvocationID] == offA);
                //assert(syrk_offB[currentInvocationID] == offB);
                //assert(syrk_offC[currentInvocationID] == offC);

                // Assign the computed offsets
                syrk_offA[currentInvocationID] = offA;
                syrk_offB[currentInvocationID] = offB;
                syrk_offC[currentInvocationID] = offC;

                currentInvocationID++;
            }
        }
    }
}

void initSyrkArgs(syrkArgs* thisSyrkArgs, int n, int nb, int invocationID) {
    if (syrkOffsetInit == 0) {
        computeSyrkOffset(n, nb);
        syrkOffsetInit = 1;
    }
    thisSyrkArgs->N = nb;
    thisSyrkArgs->K = nb;
    thisSyrkArgs->alpha = -1.0;
    thisSyrkArgs->A = 0;
    thisSyrkArgs->lda = n;
    thisSyrkArgs->beta = 1.0;
    thisSyrkArgs->C = 0;
    thisSyrkArgs->ldc = n;
    thisSyrkArgs->startN = 0;
    thisSyrkArgs->origN = nb;
    thisSyrkArgs->offA = syrk_offA[invocationID];
    thisSyrkArgs->offB = syrk_offB[invocationID];
    thisSyrkArgs->offC = syrk_offC[invocationID];
}
