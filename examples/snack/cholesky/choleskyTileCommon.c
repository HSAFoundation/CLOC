#include "choleskyTile.h"
#include "magmaWrapper.h"

void connectSignal(hsa_signal_t signal, hsa_dispatch_packet_t* aql) {
   aql->completion_signal=signal;
}
/*
 * Determines if a memory region can be used for kernarg
 * allocations.
 */
hsa_status_t get_kernarg(hsa_region_t region, void* data) {
    hsa_region_flag_t flags;
    hsa_region_get_info(region, HSA_REGION_INFO_FLAGS, &flags);
    if (flags & HSA_REGION_FLAG_KERNARG) {
        hsa_region_t* ret = (hsa_region_t*) data;
        *ret = region;
    }
    return HSA_STATUS_SUCCESS;
}

void copyBlock(float* host, uint offsetHost, float* dest, uint offsetDest, uint ldh, uint ldd) {
    float* baseaddrHost = host + offsetHost;
    float* baseaddrDest = dest + offsetDest;
    int rowIter, colIter;
    int blockDim = min( ldh, ldd );
    
    for (rowIter = 0; rowIter < blockDim; rowIter++) {
        for (colIter = 0; colIter < blockDim; colIter++) {
            *(baseaddrDest + rowIter* ldd + colIter) = *(baseaddrHost + rowIter * ldh + colIter);
        }
    }
}
