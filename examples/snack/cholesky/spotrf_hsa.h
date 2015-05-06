#include "choleskyTile.h"
#include "magma_types_magmaWrapper.h"
void spotrf_hsa(magma_uplo_t uplo, magma_int_t n, float* dA, size_t dA_offset, magma_int_t ldda, magma_int_t* info);
// produces pointer and offset as two args to magmaBLAS routines
#define dA(i,j)  dA, ( (dA_offset) + (i) + (j)*ldda )
