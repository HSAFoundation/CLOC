#include "spotrf_hsa.h"
#include "createAQLPacket.h"
#include "lapacke.h"
#include <cblas.h>
#include "spotrf_dependency_list.h"

extern hsa_queue_t* taskingCommandQueue[30];
extern hsa_ext_program_handle_t taskingSyrkProgram;
extern hsa_ext_code_descriptor_t *taskingSyrkCodeDescriptor;
extern hsa_ext_program_handle_t taskingGemmProgram;
extern hsa_ext_code_descriptor_t *taskingGemmCodeDescriptor;
extern hsa_ext_program_handle_t taskingTrsmProgram;
extern hsa_ext_code_descriptor_t *taskingTrsmCodeDescriptor;
extern hsa_dispatch_packet_t taskingAQLGemm[10];
extern hsa_dispatch_packet_t taskingAQLSyrk[10];
extern hsa_dispatch_packet_t taskingAQLTrsm[10];
extern hsa_status_t taskingErr;

int64_t atomic_load(int64_t* p) {
    return __atomic_load_n (p, __ATOMIC_SEQ_CST);
}

void spinLoop(void* ptr, uint64_t compare_value) {
            while (atomic_load((uint64_t*)ptr + 1) >= compare_value) {
               //spin loop
            }
}

static void loadFile(int kernelInvokationID, magma_int_t n, float* dA, size_t dA_offset, magma_int_t ldda) {
    FILE* inputTxt;
    char inputbuffer[50];
    int fileIndex;
    if (ldda != n) {
        printf("ldda = %d, n = %d\n",ldda, n);
        exit(0);
    }
    int unused=sprintf(inputbuffer, "inputFile%d",kernelInvokationID);
    //************ Loading the correct array ***********//
    inputTxt = fopen(inputbuffer,"r");
    for (fileIndex = 0; fileIndex < n * n; fileIndex++) {
        fscanf(inputTxt,"%f",&dA[fileIndex]);
    }
    fclose(inputTxt);
}

static void printFile(int kernelInvokationID, magma_int_t n, float* dA, size_t dA_offset, magma_int_t ldda) {
    FILE* outputTxt;
    char outputbuffer[50];
    int fileIndex;
    if (ldda != n) {
        printf("ldda = %d, n = %d\n",ldda, n);
        exit(0);
    }
    //float* work = (float*) malloc(sizeof(float) * n * n);

    int unused=sprintf(outputbuffer, "outputFile%d",kernelInvokationID);

    //************ Dumpiong the generated array ***********//
    //copyBlock(dA, dA_offset, work, 0, ldda, n);
    outputTxt = fopen(outputbuffer,"w+");
    for (fileIndex = 0; fileIndex < n * n; fileIndex++) {
        fprintf(outputTxt,"%f\n",dA[fileIndex]);
    }
    fclose(outputTxt);
    //free(work);
}


static magma_int_t magma_get_spotrf_nb( magma_int_t m )
{
    //if      (m <= 1024) return 128;
    //else                return 320;
    assert(m % 5 == 0); // we are implementing 5x5 tiling only
    int n = m / 5;
    assert(n % 128 == 0); // Kernels only support multiples of 128 blocking 
    return n;
}

static void enqueuePacket(hsa_dispatch_packet_t aql, hsa_queue_t* commandQueue) { 
    /* 
     * Obtain the current queue write index.
     */ 
    uint64_t index = hsa_queue_load_write_index_relaxed(commandQueue); 

    /*
     * Write the aql packet at the calculated queue index address.
     */ 
    const uint32_t queueMask = commandQueue->size - 1;
    ((hsa_dispatch_packet_t*)(commandQueue->base_address))[index&queueMask]=aql;

    /*
     * Increment the write index and ring the doorbell to dispatch the kernel.
     */
    hsa_queue_store_write_index_relaxed(commandQueue, index+1);
    hsa_signal_store_relaxed(commandQueue->doorbell_signal, index);
}

void enqueueBarrierPacket(hsa_barrier_packet_t aql, hsa_queue_t* commandQueue) { 
    /* 
     * Obtain the current queue write index.
     */ 
    uint64_t index = hsa_queue_load_write_index_relaxed(commandQueue); 

    /*
     * Write the aql packet at the calculated queue index address.
     */ 
    const uint32_t queueMask = commandQueue->size - 1;
    ((hsa_barrier_packet_t*)(commandQueue->base_address))[index&queueMask]=aql;

    /*
     * Increment the write index and ring the doorbell to dispatch the kernel.
     */
    hsa_queue_store_write_index_relaxed(commandQueue, index+1);
    hsa_signal_store_relaxed(commandQueue->doorbell_signal, index);
}

static void syncGPU(hsa_signal_t signal) {
    /*
     * Wait on the dispatch signal until the kernel is finished.
     */
    taskingErr = hsa_signal_wait_acquire(signal, HSA_LT, 1, (uint64_t) -1, HSA_WAIT_EXPECTANCY_UNKNOWN);
    check(Wating on the dispatch signal, taskingErr);
}

void doSyrk(int invocationID, int commadQueueID, float* dA,  hsa_signal_t completionSignal, int numDeps, hsa_signal_t* depSignals, int n, int nb) {
    printf("Starting syrk[%d]\n",invocationID);
    if (numDeps > 0) { 
        hsa_barrier_packet_t barrierPacket;
        memset(&barrierPacket, 0, sizeof(barrierPacket));
        createBarrierAQLPacket(&barrierPacket, numDeps, depSignals);
        enqueueBarrierPacket(barrierPacket, taskingCommandQueue[commadQueueID]);
    }
    createSyrkAQLPacket(completionSignal, &taskingAQLSyrk[invocationID], taskingSyrkCodeDescriptor, dA, n, nb, invocationID, 0);
    enqueuePacket(taskingAQLSyrk[invocationID], taskingCommandQueue[commadQueueID]);
}

void doGemm(int invocationID, int commadQueueID, float* dA,  hsa_signal_t completionSignal, int numDeps, hsa_signal_t* depSignals, int n, int nb) {
    printf("Starting gemm[%d]\n",invocationID);
    if (numDeps > 0) { 
        hsa_barrier_packet_t barrierPacket;
        memset(&barrierPacket, 0, sizeof(barrierPacket));
        createBarrierAQLPacket(&barrierPacket, numDeps, depSignals);
        enqueueBarrierPacket(barrierPacket, taskingCommandQueue[commadQueueID]);
    }
    createGemmAQLPacket(completionSignal, &taskingAQLGemm[invocationID], taskingGemmCodeDescriptor, dA, n, nb, invocationID, 0);
    enqueuePacket(taskingAQLGemm[invocationID], taskingCommandQueue[commadQueueID]);
}

void doTrsm(int invocationID, int commadQueueID, float* dA,  hsa_signal_t completionSignal, int numDeps, hsa_signal_t* depSignals, int n, int nb) {
    printf("Starting trsm[%d]\n",invocationID);
    if (numDeps > 0) { 
        hsa_barrier_packet_t barrierPacket;
        memset(&barrierPacket, 0, sizeof(barrierPacket));
        createBarrierAQLPacket(&barrierPacket, numDeps, depSignals);
        enqueueBarrierPacket(barrierPacket, taskingCommandQueue[commadQueueID]);
    }
    createTrsmAQLPacket(completionSignal, &taskingAQLTrsm[invocationID], taskingTrsmCodeDescriptor, dA, n, nb, invocationID, 0);
    enqueuePacket(taskingAQLTrsm[invocationID], taskingCommandQueue[commadQueueID]);
}

void doPotrf(int invocationID, int commadQueueID, float* dA,  size_t dA_offset, magma_int_t ldda, hsa_signal_t completionSignal, int numDeps, hsa_signal_t* depSignals, magma_int_t* info, magma_int_t n) {
    assert(numDeps == 1 || numDeps == 0); // potrf can have max 1 dependency 
    if (numDeps == 1)  { 
        //syncGPU(depSignals[0]);
        spinLoop((void *)depSignals[0], 1);
    }
    int jb = magma_get_spotrf_nb( n );
    assert(jb%128 == 0);
    float* work = (float*) malloc(jb * jb * sizeof(float));
    int j = jb * invocationID;
    printf("Starting spotrf[%d]\n",invocationID);
    copyBlock(dA(j,j), work, 0, n, jb);
    LAPACK_spotrf( MagmaLowerStr, &jb, work, &jb, info );
    if ( *info != 0 ) {
        assert( *info > 0 );
        *info += j;
    }
    copyBlock(work, 0, dA(j,j), jb, n);
    hsa_signal_store_relaxed(completionSignal, 0);
    free(work);
}

void connectDepSignals(depContainer* thisDepContainer, int numDeps, hsa_signal_t sig1, hsa_signal_t sig2, hsa_signal_t sig3) {
    thisDepContainer->numDeps = numDeps;
    thisDepContainer->sig[0] = sig1;
    thisDepContainer->sig[1] = sig2;
    thisDepContainer->sig[2] = sig3;
}

void spotrf_task(magma_uplo_t uplo, magma_int_t n, float* dA, size_t dA_offset, magma_int_t ldda, magma_int_t* info)
{
    magma_int_t j, jb, nb;
    magma_int_t taskingErr;
    *info = 0;
    nb = magma_get_spotrf_nb( n );
    hsa_signal_t syrkSignal[10];
    hsa_signal_t gemmSignal[10];
    hsa_signal_t trsmSignal[10];
    hsa_signal_t potrfSignal[5];
    int signalIter = 0;
    //***** Creating completion signals
    for (signalIter = 0; signalIter < 10; signalIter++) {
        taskingErr=hsa_signal_create(1, 0, NULL, &syrkSignal[signalIter]);
        check(Creating a syrk signal, taskingErr);
    }
    for (signalIter = 0; signalIter < 10; signalIter++) {
        taskingErr=hsa_signal_create(1, 0, NULL, &gemmSignal[signalIter]);
        check(Creating a gemm signal, taskingErr);
    }
    for (signalIter = 0; signalIter < 10; signalIter++) {
        taskingErr=hsa_signal_create(1, 0, NULL, &trsmSignal[signalIter]);
        check(Creating a trsm signal, taskingErr);
    }
    for (signalIter = 0; signalIter < 5; signalIter++) {
        taskingErr=hsa_signal_create(1, 0, NULL, &potrfSignal[signalIter]);
        check(Creating a potrf signal, taskingErr);
    }

    depContainer tmpDepContainer;
    
    // Start tasking cholesky

    // Level 1 parallelization
    connectDepSignals(T0_C);
    DO_TRSM(0, 0);

    // Level 0 parallelization
    connectDepSignals(P0_C);
    DO_POTRF(0,0);

    // Level 2 parallelization
    connectDepSignals(S0_C);
    DO_SYRK(0, 4);

    connectDepSignals(T1_C);
    DO_TRSM(1, 1);

    connectDepSignals(T2_C);
    DO_TRSM(2, 2);

    connectDepSignals(T3_C);
    DO_TRSM(3, 3);

    // Level 3 parallelization
    connectDepSignals(P1_C);
    DO_POTRF(1, 0);

    connectDepSignals(G0_C);
    DO_GEMM(0, 5);

    connectDepSignals(S3_C);
    DO_SYRK(3, 6);

    connectDepSignals(S6_C);
    DO_SYRK(6, 7);

    connectDepSignals(G7_C);
    DO_GEMM(7, 8);

    connectDepSignals(G1_C);
    DO_GEMM(1, 9);

    connectDepSignals(G3_C);
    DO_GEMM(3, 0);

    connectDepSignals(G5_C);
    DO_GEMM(5, 1);

    connectDepSignals(G2_C);
    DO_GEMM(2, 2);

    connectDepSignals(S1_C);
    DO_SYRK(1, 3);


    // Level 4 parallelization
    connectDepSignals(T5_C);
    DO_TRSM(5, 4);

    connectDepSignals(T4_C);
    DO_TRSM(4, 5);

    connectDepSignals(T6_C);
    DO_TRSM(6, 6);

    // Level 5 parallelization
    connectDepSignals(S4_C);
    DO_SYRK(4, 7);

    connectDepSignals(G4_C);
    DO_GEMM(4, 1);

    connectDepSignals(S2_C);
    DO_SYRK(2, 2);

    connectDepSignals(G8_C);
    DO_GEMM(8, 3);

    connectDepSignals(G6_C);
    DO_GEMM(6, 4);

    connectDepSignals(S7_C);
    DO_SYRK(7, 5);

    // Level 7 parallelization
    connectDepSignals(T7_C);
    DO_TRSM(7, 2);

    connectDepSignals(T8_C);
    DO_TRSM(8, 3);

    // Level 8 parallelization
    connectDepSignals(S5_C);
    DO_SYRK(5, 1);

    connectDepSignals(G9_C);
    DO_GEMM(9, 2);

    connectDepSignals(S8_C);
    DO_SYRK(8, 3);


    // Level 10 parallelization
    connectDepSignals(T9_C);
    DO_TRSM(9, 1);

    // Level 11 parallelization
    connectDepSignals(S9_C);
    DO_SYRK(9, 1);

    // Level 6 parallelization
    connectDepSignals(P2_C);
    DO_POTRF(2, 2);
    // Level 9 parallelization
    connectDepSignals(P3_C);
    DO_POTRF(3, 1);
    // Level 12 parallelization
    connectDepSignals(P4_C);
    DO_POTRF(4, 1);


    //***** Destroying completion signals
    for (signalIter = 0; signalIter < 10; signalIter++) {
        taskingErr=hsa_signal_destroy(syrkSignal[signalIter]);
        check(Destroying syrk signal, taskingErr);
    }
    for (signalIter = 0; signalIter < 10; signalIter++) {
        taskingErr=hsa_signal_destroy(gemmSignal[signalIter]);
        check(Destroying gemm signal, taskingErr);
    }
    for (signalIter = 0; signalIter < 10; signalIter++) {
        taskingErr=hsa_signal_destroy(trsmSignal[signalIter]);
        check(Destroying trsm signal, taskingErr);
    }
    for (signalIter = 0; signalIter < 5; signalIter++) {
        taskingErr=hsa_signal_destroy(potrfSignal[signalIter]);
        check(Destroying potrf signal, taskingErr);
    }
}
