#include "spotrf_hsa.h"
#include "createAQLPacket.h"
#include "lapacke.h"
#include "cblas.h"

extern hsa_queue_t* commandQueue;
extern hsa_ext_program_handle_t syrkProgram;
extern hsa_ext_code_descriptor_t *syrkCodeDescriptor;
extern hsa_ext_program_handle_t gemmProgram;
extern hsa_ext_code_descriptor_t *gemmCodeDescriptor;
extern hsa_ext_program_handle_t trsmProgram;
extern hsa_ext_code_descriptor_t *trsmCodeDescriptor;
extern hsa_dispatch_packet_t aqlGemm[10];
extern hsa_dispatch_packet_t aqlSyrk[10];
extern hsa_dispatch_packet_t aqlTrsm[10];
extern hsa_status_t err;

void loadFile(int kernelInvokationID, magma_int_t n, float* dA, size_t dA_offset, magma_int_t ldda) {
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

void printFile(int kernelInvokationID, magma_int_t n, float* dA, size_t dA_offset, magma_int_t ldda) {
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


magma_int_t magma_get_spotrf_nb( magma_int_t m )
{
    //if      (m <= 1024) return 128;
    //else                return 320;
    assert(m % 5 == 0); // we are implementing 5x5 tiling only
    int n = m / 5;
    assert(n % 128 == 0); // Kernels only support multiples of 128 blocking 
    return n;
}

void enqueuePacket(hsa_dispatch_packet_t aql, hsa_queue_t* commandQueue) { 
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

void syncGPU(hsa_signal_t signal) {
    /*
     * Wait on the dispatch signal until the kernel is finished.
     */
    err = hsa_signal_wait_acquire(signal, HSA_LT, 1, (uint64_t) -1, HSA_WAIT_EXPECTANCY_UNKNOWN);
    check(Wating on the dispatch signal, err);
}


void spotrf_hsa(magma_uplo_t uplo, magma_int_t n, float* dA, size_t dA_offset, magma_int_t ldda, magma_int_t* info) {
    magma_int_t j, jb, nb;
    magma_int_t err;
    *info = 0;
    nb = magma_get_spotrf_nb( n );
    float* work = (float*) malloc(nb * nb * sizeof(float)); 
    float* work2 = (float*) malloc(nb * nb * sizeof(float)); 
    int syrkInvocation = 0;
    int gemmInvocation = 0;
    int trsmInvocation = 0;
    int kernelInvokationID = 0;
    //printFile(kernelInvokationID, n, dA, dA_offset, ldda);
    kernelInvokationID++;
    /*
     * Declare signal and create a dummy signal to connext to all packets but the last.
     */
    //hsa_signal_t signal;
    //hsa_signal_t dummy_signal;
    //err=hsa_signal_create(1, 0, NULL, &dummy_signal);
    //check(Creating a HSA signal, err);
    /* --------------------
     compute Cholesky factorization A = L*L'
     using the left looking algorithm*/ 
    for( j = 0; j < n; j += nb ) { 
        /* apply all previous updates to diagonal block */
        //jb = min( nb, n-j );
        jb = nb;
        assert( nb <= n-j);
        if ( j>0 ) {
            printf("Starting ssyrk\n");
            int tile_row = j;
            int tile_col = j;
            int col_iter;
            for (col_iter = 0; col_iter < tile_col; col_iter += nb) {
                hsa_signal_t syrkSignal;
                err=hsa_signal_create(1, 0, NULL, &syrkSignal);
                check(Creating a syrk signal, err);
                createSyrkAQLPacket(syrkSignal, &aqlSyrk[syrkInvocation], syrkCodeDescriptor, dA, n, nb, syrkInvocation, 1);
                enqueuePacket(aqlSyrk[syrkInvocation], commandQueue);
                syrkInvocation++;
                syncGPU(syrkSignal);
                err=hsa_signal_destroy(syrkSignal);
                check(Destroying syrk signal, err);
                //printFile(kernelInvokationID, n, dA, dA_offset, ldda);
                //loadFile(kernelInvokationID, n, dA, dA_offset, ldda);
                //printf("kernelInvokationID = %d : syrk\n", kernelInvokationID);
                kernelInvokationID++;
            }
        }

        /* apply all previous updates to block column below diagonal block */
        if ( j+jb < n && j > 0) {
            printf("Starting sgemm\n");
            int tile_row;
            int tile_col = j;
            int col_iter;
            for (tile_row = j + jb; tile_row < n; tile_row += nb) {
                for (col_iter = 0; col_iter < tile_col; col_iter += nb) {
                    hsa_signal_t gemmSignal;
                    err=hsa_signal_create(1, 0, NULL, &gemmSignal);
                    check(Creating a gemm signal, err);
                    createGemmAQLPacket(gemmSignal, &aqlGemm[gemmInvocation], gemmCodeDescriptor, dA, n, nb, gemmInvocation, 1);
                    enqueuePacket(aqlGemm[gemmInvocation], commandQueue);
                    gemmInvocation++;
                    syncGPU(gemmSignal);
                    err=hsa_signal_destroy(gemmSignal);
                    check(Destroying gemm signal, err);
                    //printFile(kernelInvokationID, n, dA, dA_offset, ldda);
                    //loadFile(kernelInvokationID, n, dA, dA_offset, ldda);
                    //printf("kernelInvokationID = %d : gemm\n", kernelInvokationID);
                    kernelInvokationID++;
                }
            }
        }
        
        /* simultaneous with above sgemm, transfer data, factor
         diagonal block on CPU, and test for positive definiteness*/
        printf("Starting matrix copy: preparing for spotrf\n");
        //if (j != 0) { 
        //    syncGPU(signal);
        //    err=hsa_signal_destroy(signal);
        //    check(Destroying the signal, err);
        //}
        copyBlock(dA(j,j), work, 0, n, jb);
        printf("Starting spotrf\n");
        LAPACK_spotrf( MagmaLowerStr, &jb, work, &jb, info );
        if ( *info != 0 ) {
            assert( *info > 0 );
            *info += j;
            break;
        }
        copyBlock(work, 0, dA(j,j), jb, n);
        //printFile(kernelInvokationID, n, dA, dA_offset, ldda);
        //loadFile(kernelInvokationID, n, dA, dA_offset, ldda);
        printf("kernelInvokationID = %d : spotrf\n", kernelInvokationID);
        kernelInvokationID++;
        //if (j+jb < n) { 
        //    /*
        //     * Create a signal to wait for the dispatch to finish.
        //     */
        //    err=hsa_signal_create(1, 0, NULL, &signal);
        //    check(Creating a HSA signal, err);
        //}
        
        /* apply diagonal block to block column below diagonal */
        if ( j+jb < n ) {
            printf("Starting strsm\n");
            int tile;
            for (tile = 0; tile < (n-j-jb)/jb; tile++) {
                    hsa_signal_t trsmSignal;
                    err=hsa_signal_create(1, 0, NULL, &trsmSignal);
                    check(Creating a trsm signal, err);
                    createTrsmAQLPacket(trsmSignal, &aqlTrsm[trsmInvocation], trsmCodeDescriptor, dA, n, nb, trsmInvocation, 1);
                    enqueuePacket(aqlTrsm[trsmInvocation], commandQueue);
                    trsmInvocation++;
                    syncGPU(trsmSignal);
                    err=hsa_signal_destroy(trsmSignal);
                    check(Destroying trsm signal, err);

                    //copyBlock(dA(j,j), work, 0, n, jb);
                    //copyBlock(dA(j+jb + tile*jb, j), work2, 0, n, jb);
                    //cblas_strsm(CblasColMajor, MagmaRight, MagmaLower, MagmaConjTrans, MagmaNonUnit,
                    //            jb, jb, 1, work, jb, work2, jb);
                    //cblas_strsm(CblasColMajor, MagmaRight, MagmaLower, MagmaConjTrans, MagmaNonUnit,
                    //            jb, jb, 1, dA + j + j * ldda, ldda, dA + (j + jb + tile*jb) + j * ldda , ldda);
                    //copyBlock(work2, 0, dA(j+jb + tile*jb, j), jb, n);

                    //printFile(kernelInvokationID, n, dA, dA_offset, ldda);
                    //loadFile(kernelInvokationID, n, dA, dA_offset, ldda);
                    printf("kernelInvokationID = %d : trsm\n", kernelInvokationID);
                    kernelInvokationID++;
            }
        }
    }
    free(work);
    free(work2);
} 
