typedef struct {
    uint N;
    uint M;
    float alpha;
    uint A;
    uint lda;
    uint B;
    uint ldb;
    uint offA;
    uint offB;
} trsmArgs;

uint trsmOffsetInit = 0;

uint trsm_offA[10] = {0,   0,   0,   0,   82048,  82048, 82048, 164096, 164096, 246144};
uint trsm_offB[10] = {128, 256, 384, 512, 82176,  82304, 82432, 164224, 164352, 246272};

void computeTrsmOffset( int n, int nb ) {
    int j;
    int currentInvocationID = 0;
    for ( j = 0; j < n; j += nb ) {
        assert( nb <= n-j );
        if( j+nb < n) {
            assert(currentInvocationID < 10);
            int tile;
            for (tile = 0; tile < (n-j-nb)/nb; tile++) {
                int offA = j + j * n;
                int offB = j + j * n + (tile + 1) * nb;

                //// To be removed later
                //assert(trsm_offA[currentInvocationID] == offA);
                //assert(trsm_offB[currentInvocationID] == offB);

                // Assign the computed offsets
                trsm_offA[currentInvocationID] = offA;
                trsm_offB[currentInvocationID] = offB;

                currentInvocationID++;
            }
        }
    }
}

void initTrsmArgs(trsmArgs* thisTrsmArgs, int n, int nb, int invocationID) {
    if (trsmOffsetInit == 0) {
        computeTrsmOffset(n, nb);
        trsmOffsetInit = 1;
    }
    thisTrsmArgs->N = nb;
    thisTrsmArgs->M = nb;
    thisTrsmArgs->alpha = 1.0;
    thisTrsmArgs->A = 0;
    thisTrsmArgs->lda = n;
    thisTrsmArgs->B = 0;
    thisTrsmArgs->ldb = n;
    thisTrsmArgs->offA = trsm_offA[invocationID];
    thisTrsmArgs->offB = trsm_offB[invocationID];
}
