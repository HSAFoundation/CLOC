#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "magmaWrapper.h"
#include "magmaWrapperExtensions.h"
#include "hsaPortCholeskyImpl.h"
#include "taskingCholeskyImpl.h"
#include "lapacke.h"
#include "cblas.h"

/* ////////////////////////////////////////////////////////////////////////////
   -- Testing spotrf
*/
int main( int argc, char** argv)
{
    //FILE* inputTxt = fopen("inputFile.txt","w+");
    //FILE* secondaryTxt = fopen("secondaryFile.txt","w+");
    //int fileIndex = 0;
    //char *mode = "w"
     
    real_Double_t   gflops, gpu_perf, gpu_time, cpu_perf, cpu_time;
    float *h_A, *h_R;
    h_A = NULL;
    h_R = NULL;
    magma_int_t N, n2, lda, ldda, info;
    float c_neg_one = MAGMA_S_NEG_ONE;
    magma_int_t ione     = 1;
    magma_int_t ISEED[4] = {0,0,0,1};
    float      work[1], error;
    magma_int_t     status = 0;

    magma_opts opts;
    parse_opts( argc, argv, &opts );
    opts.lapack |= opts.check;  // check (-c) implies lapack (-l)
    
    float tol = opts.tolerance * LAPACK_slamch((char *)"E");
    printf("Tolerance = %f\n", tol);
    printf("uplo = %s, version = %d\n", lapack_uplo_const(opts.uplo), opts.version );
    for( int itest = 0; itest < opts.ntest; ++itest ) {
        for( int iter = 0; iter < opts.niter; ++iter ) {
            N   = opts.nsize[itest];
            lda = N;
            n2  = lda*N;
            ldda = ((N+31)/32)*32;
            gflops = FLOPS_SPOTRF( N ) / 1e9;
 
            h_A = (float*) malloc(n2 * sizeof(float));
            h_R = (float*) malloc(n2 * sizeof(float));

            /* Initialize the matrix */
            //LAPACK_slarnv( &ione, ISEED, &n2, h_A );
            slarnv_( &ione, ISEED, &n2, h_R );
            magma_smake_hpd( N, h_R, lda );
int test = 0;
for (int test = 0; test < 1000; test++) {
            slacpy_( (char *)MagmaUpperLowerStr, &N, &N, h_R, &lda, h_A, &lda );
 
            //fwrite(h_A, sizeof(float), sizeof(float)*n2, inputTxt);
            //for (fileIndex = 0; fileIndex < n2; fileIndex++) {
            //    fprintf(inputTxt,"%f\n",h_A[fileIndex]);
            //}
            //fclose(inputTxt);
            /* ====================================================================
               Performs operation using MAGMA
               =================================================================== */
            real_Double_t timeThisIter;
            timeThisIter = magma_wtime();
            if ( opts.version == 1 ) {
                //LAPACK_spotrf( (char*)lapack_uplo_const(opts.uplo), &N, h_A, &lda, &info );
                hsaPortCholeskyImpl( opts.uplo, N, h_A, 0, lda, &info );
            }
            else if ( opts.version == 2 ) {
                taskingCholeskyImpl( opts.uplo, N, h_A, 0, lda, &info, test == 9 );
                //magma_spotrf2_gpu( opts.uplo, N, d_A, 0, ldda, opts.queues2, &info );
            }
            else {
                printf( "Unknown version %d\n", opts.version );
                exit(1);
            }
            timeThisIter = magma_wtime() - timeThisIter;
            if (test == 0) {
                gpu_time = timeThisIter;
            } else if (timeThisIter < gpu_time) {
                gpu_time = timeThisIter;
            }
            gpu_perf = gflops / gpu_time;
}
            if (info != 0)
                printf("magma_spotrf_gpu returned error %d: %s.\n",
                       (int) info, magma_strerror( info ));
            
            if ( opts.lapack ) {
                /* =====================================================================
                   Performs operation using LAPACK
                   =================================================================== */
                cpu_time = magma_wtime();
                LAPACK_spotrf( (char*)lapack_uplo_const(opts.uplo), &N, h_R, &lda, &info );
                //inputTxt = fopen("inputFile.txt","r");
                //for (fileIndex = 0; fileIndex < n2; fileIndex++) {
                //    fscanf(inputTxt,"%f",&h_R[fileIndex]);
                //    fprintf(secondaryTxt,"%f\n",h_R[fileIndex]);
                //}
                //fclose(inputTxt);
                //fclose(secondaryTxt);
                cpu_time = magma_wtime() - cpu_time;
                cpu_perf = gflops / cpu_time;
                if (info != 0)
                    printf("lapackf77_spotrf returned error %d: %s.\n",
                           (int) info, magma_strerror( info ));
                
                /* =====================================================================
                   Check the result compared to LAPACK
                   =================================================================== */
                error = LAPACK_slange((char *)"f", &N, &N, h_A, &lda, work);
                cblas_saxpy(n2, c_neg_one, h_A, ione, h_R, ione);
                error = LAPACK_slange((char *)"f", &N, &N, h_R, &lda, work) / error;
                
                printf("  N     CPU GFlop/s (sec)   GPU GFlop/s (sec)   ||R_magma - R_lapack||_F / ||R_lapack||_F\n");
                printf("========================================================\n");
                printf("%5d   %7.6f (%7.6f)   %7.6f (%7.6f)   %8.2e   %s\n",
                       (int) N, cpu_perf, cpu_time, gpu_perf, gpu_time,
                       error, (error < tol ? "ok" : "failed") );
                status += ! (error < tol);
            }
            else {
                printf("  N     CPU GFlop/s (sec)   GPU GFlop/s (sec)   ||R_magma - R_lapack||_F / ||R_lapack||_F\n");
                printf("========================================================\n");
                printf("%5d     ---   (  ---  )   %7.2f (%7.2f)     ---  \n",
                       (int) N, gpu_perf, gpu_time );
            }
            free( h_A );
            free( h_R );
            fflush( stdout );
        }
        if ( opts.niter > 1 ) {
            printf( "\n" );
        }
    }

    return status;
}
