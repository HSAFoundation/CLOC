#include <string.h>
#include <assert.h>
#include <sys/file.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


#include "flops_magmaWrapper.h"
#include "magma_types_magmaWrapper.h"

#define MAX_NTEST 256
#define magma_num_gpus() 1 
/***************************************************************************//**
 *  Global utilities
 *  in both common_magma.h and testings.h
 **/
#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef roundup
#define roundup(a, b) (b <= 0) ? (a) : (((a) + (b)-1) & ~((b)-1))
#endif

#ifndef ceildiv
#define ceildiv(a, b) ((a - 1)/b + 1)
#endif

typedef struct magma_opts
{
    // matrix size
    magma_int_t ntest;
    magma_int_t msize[ MAX_NTEST ];
    magma_int_t nsize[ MAX_NTEST ];
    magma_int_t ksize[ MAX_NTEST ];
    magma_int_t mmax;
    magma_int_t nmax;
    magma_int_t kmax;
    
    // scalars
    magma_int_t device;
    magma_int_t pad;
    magma_int_t nb;
    magma_int_t nrhs;
    magma_int_t nstream;
    magma_int_t ngpu;
    magma_int_t nsub;
    magma_int_t niter;
    magma_int_t nthread;
    magma_int_t offset;
    magma_int_t itype;     // hegvd: problem type
    magma_int_t svd_work;  // gesvd
    magma_int_t version;   // hemm_mgpu, hetrd
    double      fraction;  // hegvdx
    double      tolerance;
    magma_int_t panel_nthread; //in magma_amc: first dimension for a 2D big panel
    double fraction_dcpu; //in magma_amc: fraction of the work for the cpu 
    // boolean arguments
    int check;
    int lapack;
    int warmup;
    int all;
    int verbose;
    
    // lapack flags
    magma_uplo_t    uplo;
    magma_trans_t   transA;
    magma_trans_t   transB;
    magma_side_t    side;
    magma_diag_t    diag;
    magma_vec_t     jobu;    // gesvd:  no left  singular vectors
    magma_vec_t     jobvt;   // gesvd:  no right singular vectors
    magma_vec_t     jobz;    // heev:   no eigen vectors
    magma_vec_t     jobvr;   // geev:   no right eigen vectors
    magma_vec_t     jobvl;   // geev:   no left  eigen vectors
    
    // misc
    int flock_op;   // shared or exclusive lock
    int flock_fd;   // lock file
} magma_opts;

static double magma_wtime( void )
{
    struct timeval t;
    gettimeofday( &t, NULL );
    return t.tv_sec + t.tv_usec*1e-6;
}
// ----------------------------------------
static const char* magma_strerror( magma_int_t error )
{
    // LAPACK-compliant errors
    if ( error > 0 ) {
        return "function-specific error, see documentation";
    }
    else if ( error < 0 && error > MAGMA_ERR ) {
        return "invalid argument";
    }
    // MAGMA-specific errors
    switch( error ) {
        case MAGMA_SUCCESS:
            return "success";
        
        case MAGMA_ERR:
            return "unknown error";
        
        case MAGMA_ERR_NOT_INITIALIZED:
            return "not initialized";
        
        case MAGMA_ERR_REINITIALIZED:
            return "reinitialized";
        
        case MAGMA_ERR_NOT_SUPPORTED:
            return "not supported";
        
        case MAGMA_ERR_ILLEGAL_VALUE:
            return "illegal value";
        
        case MAGMA_ERR_NOT_FOUND:
            return "not found";
        
        case MAGMA_ERR_ALLOCATION:
            return "allocation";
        
        case MAGMA_ERR_INTERNAL_LIMIT:
            return "internal limit";
        
        case MAGMA_ERR_UNALLOCATED:
            return "unallocated error";
        
        case MAGMA_ERR_FILESYSTEM:
            return "filesystem error";
        
        case MAGMA_ERR_UNEXPECTED:
            return "unexpected error";
        
        case MAGMA_ERR_SEQUENCE_FLUSHED:
            return "sequence flushed";
        
        case MAGMA_ERR_HOST_ALLOC:
            return "cannot allocate memory on CPU host";
        
        case MAGMA_ERR_DEVICE_ALLOC:
            return "cannot allocate memory on GPU device";
        
        case MAGMA_ERR_CUDASTREAM:
            return "CUDA stream error";
        
        case MAGMA_ERR_INVALID_PTR:
            return "invalid pointer";
        
        default:
            return "unknown MAGMA error code";
    }
}
