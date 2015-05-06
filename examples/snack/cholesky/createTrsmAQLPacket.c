#include "choleskyTile.h"
#include "trsmArgList.h"

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
    uint N;
    uint M;
    float alpha;
    float *A;
    uint lda;
    float *B;
    uint ldb;
    uint offA;
    uint offB;
} trsmKArgs;

void setupTrsmAqlDispatchInfo(hsa_dispatch_packet_t* aql, hsa_signal_t signal, int gridSize, int ordered) {
    /*
     * Setup the dispatch information.
     */
    aql->completion_signal=signal;
    aql->dimensions=1;
    aql->workgroup_size_x=64;
    aql->workgroup_size_y=1;
    aql->workgroup_size_z=1;
    aql->grid_size_x=gridSize;
    aql->grid_size_y=1;
    aql->grid_size_z=1;
    aql->header.type=HSA_PACKET_TYPE_DISPATCH;
    aql->header.acquire_fence_scope=2;
    aql->header.release_fence_scope=2;
    aql->header.barrier=ordered;
    aql->group_segment_size=0;
    aql->private_segment_size=0;
}

void setupTrsmKernelArgs(trsmKArgs* args, trsmArgs* thisTrsmArgs, float* matrix) {
    args->arg0=0 ; /* Dummy args required by compiler */
    args->arg1=0 ; /* Dummy args required by compiler */
    args->arg2=0 ; /* Dummy args required by compiler */
    args->arg3=0 ; /* Dummy args required by compiler */
    args->arg4=0 ; /* Dummy args required by compiler */
    args->arg5=0 ; /* Dummy args required by compiler */
    args->N = thisTrsmArgs->N;
    args->M = thisTrsmArgs->M;
    args->alpha = thisTrsmArgs->alpha;
    args->A = matrix + thisTrsmArgs->A;
    args->lda = thisTrsmArgs->lda;
    args->B = matrix + thisTrsmArgs->B;
    args->ldb = thisTrsmArgs->ldb;
    args->offA = thisTrsmArgs->offA;
    args->offB = thisTrsmArgs->offB;
}

void loadTrsmKernelArgs(trsmKArgs* args, hsa_dispatch_packet_t* aql, hsa_ext_code_descriptor_t* hsaCodeDescriptor) {
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
    memcpy(kernel_arg_buffer, args, sizeof(trsmKArgs));
 
    /*
     * Bind kernel code and the kernel argument buffer to the
     * aql packet.
     */
    aql->kernel_object_address=hsaCodeDescriptor->code.handle;
    aql->kernarg_address=(uint64_t)kernel_arg_buffer;

    /*
     * Register the memory region for the argument buffer.
     */
    err = hsa_memory_register(args, sizeof(trsmKArgs));
    check(Registering the argument buffer, err);
}

void createTrsmAQLPacket(hsa_signal_t signal, hsa_dispatch_packet_t* aql, hsa_ext_code_descriptor_t* hsaCodeDescriptor, float* matrix, int n, int nb, int invocationID, int ordered) {
    trsmArgs thisTrsmArgs;
    int gridSize = nb * 2;
    setupTrsmAqlDispatchInfo(aql, signal, gridSize, ordered);
    initTrsmArgs(&thisTrsmArgs, n, nb, invocationID);
    trsmKArgs args;
    setupTrsmKernelArgs(&args, &thisTrsmArgs, matrix);
    loadTrsmKernelArgs(&args, aql, hsaCodeDescriptor);
}
