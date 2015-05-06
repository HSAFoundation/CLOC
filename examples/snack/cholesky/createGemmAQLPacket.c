#include "choleskyTile.h"
#include "gemmArgList.h"

extern hsa_agent_t device;
extern hsa_status_t err;

/*** Kernel argument list ***/
typedef struct __attribute__ ((aligned(HSA_ARGUMENT_ALIGN_BYTES))) args_t {
    uint64_t arg0;
    uint64_t arg1;
    uint64_t arg2;
    uint64_t arg3;
    uint64_t arg4;
    uint64_t arg5;
    uint M;
    uint N;
    uint K;
    float alpha;
    float beta;
    float* A;
    float* B;
    float* C;
    uint lda;
    uint ldb;
    uint ldc;
    uint offA;
    uint offB;
    uint offC;
} gemmKArgs;

void setupGemmAqlDispatchInfo(hsa_dispatch_packet_t* aql, hsa_signal_t signal, int gridSize, int ordered) {
    /*
     * Setup the dispatch information.
     */
    aql->completion_signal=signal;
    aql->dimensions=2;
    aql->workgroup_size_x=8;
    aql->workgroup_size_y=8;
    aql->workgroup_size_z=1;
    aql->grid_size_x=gridSize;
    aql->grid_size_y=gridSize;
    aql->grid_size_z=1;
    aql->header.type=HSA_PACKET_TYPE_DISPATCH;
    aql->header.acquire_fence_scope=2;
    aql->header.release_fence_scope=2;
    aql->header.barrier=ordered;
    aql->group_segment_size=0;
    aql->private_segment_size=0;
}

void setupGemmKernelArgs(gemmKArgs* args, gemmArgs* thisGemmArgs, float* matrix) {
    args->arg0=0 ; /* Dummy args required by compiler */
    args->arg1=0 ; /* Dummy args required by compiler */
    args->arg2=0 ; /* Dummy args required by compiler */
    args->arg3=0 ; /* Dummy args required by compiler */
    args->arg4=0 ; /* Dummy args required by compiler */
    args->arg5=0 ; /* Dummy args required by compiler */
    args->M = thisGemmArgs->M;
    args->N = thisGemmArgs->N;
    args->K = thisGemmArgs->K;
    args->alpha = thisGemmArgs->alpha;
    args->beta = thisGemmArgs->beta;
    args->A = matrix + thisGemmArgs->A;
    args->B = matrix + thisGemmArgs->B;
    args->C = matrix + thisGemmArgs->C;
    args->lda = thisGemmArgs->lda;
    args->ldb = thisGemmArgs->ldb;
    args->ldc = thisGemmArgs->ldc;
    args->offA = thisGemmArgs->offA;
    args->offB = thisGemmArgs->offB;
    args->offC = thisGemmArgs->offC;
}

void loadGemmKernelArgs(gemmKArgs* args, hsa_dispatch_packet_t* aql, hsa_ext_code_descriptor_t* hsaCodeDescriptor) {
    /*
     * Find a memory region that supports kernel arguments.
     */
    hsa_region_t kernarg_region = 0;
    hsa_agent_iterate_regions(device, get_kernarg, &kernarg_region);
    err = (kernarg_region == 0) ? HSA_STATUS_ERROR : HSA_STATUS_SUCCESS;
    check(Finding a kernarg memory region, err);
    void* kernel_arg_buffer = NULL;
   
    size_t kernel_arg_buffer_size = hsaCodeDescriptor->kernarg_segment_byte_size;

    /*
     * Allocate the kernel argument buffer from the correct region.
     */   
    err = hsa_memory_allocate(kernarg_region, kernel_arg_buffer_size, 
                        &kernel_arg_buffer);
    check(Allocating kernel argument memory buffer, err);
    memcpy(kernel_arg_buffer, args, sizeof(gemmKArgs));
 
    /*
     * Bind kernel code and the kernel argument buffer to the
     * aql packet.
     */
    aql->kernel_object_address=hsaCodeDescriptor->code.handle;
    aql->kernarg_address=(uint64_t)kernel_arg_buffer;

    /*
     * Register the memory region for the argument buffer.
     */
    err = hsa_memory_register(args, sizeof(gemmKArgs));
    check(Registering the argument buffer, err);
}

void createGemmAQLPacket(hsa_signal_t signal, hsa_dispatch_packet_t* aql, hsa_ext_code_descriptor_t* hsaCodeDescriptor, float* matrix, int n, int nb, int invocationID, int ordered) {
    gemmArgs thisGemmArgs;
    int gridSize = nb/8;
    setupGemmAqlDispatchInfo(aql, signal, gridSize, ordered);
    initGemmArgs(&thisGemmArgs, n, nb, invocationID);
    gemmKArgs args;
    setupGemmKernelArgs(&args, &thisGemmArgs, matrix);
    loadGemmKernelArgs(&args, aql, hsaCodeDescriptor);
}
