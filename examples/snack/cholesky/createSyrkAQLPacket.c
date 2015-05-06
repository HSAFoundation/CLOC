#include "choleskyTile.h"
#include "syrkArgList.h"

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
    uint K;
    float alpha;
    float *A;
    uint lda;
    float beta;
    float *C;
    uint ldc;
    uint startN;
    uint origN;
    uint offA;
    uint offB;
    uint offC;
} syrkKArgs;

void setupSyrkAqlDispatchInfo(hsa_dispatch_packet_t* aql, hsa_signal_t signal, int gridSize, int ordered) {
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

void setupSyrkKernelArgs(syrkKArgs* args, syrkArgs* thisSyrkArgs, float* matrix) {
    args->arg0=0 ; /* Dummy args required by compiler */
    args->arg1=0 ; /* Dummy args required by compiler */
    args->arg2=0 ; /* Dummy args required by compiler */
    args->arg3=0 ; /* Dummy args required by compiler */
    args->arg4=0 ; /* Dummy args required by compiler */
    args->arg5=0 ; /* Dummy args required by compiler */
    args->N = thisSyrkArgs->N;
    args->K = thisSyrkArgs->K;
    args->alpha = thisSyrkArgs->alpha;
    args->A = matrix + thisSyrkArgs->A;
    args->lda = thisSyrkArgs->lda;
    args->beta = thisSyrkArgs->beta;
    args->C = matrix + thisSyrkArgs->C;
    args->ldc = thisSyrkArgs->ldc;
    args->startN = thisSyrkArgs->startN;
    args->origN = thisSyrkArgs->origN;
    args->offA = thisSyrkArgs->offA;
    args->offB = thisSyrkArgs->offB;
    args->offC = thisSyrkArgs->offC;
}

void loadSyrkKernelArgs(syrkKArgs* args, hsa_dispatch_packet_t* aql, hsa_ext_code_descriptor_t* hsaCodeDescriptor) {
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
    memcpy(kernel_arg_buffer, args, sizeof(syrkKArgs));
 
    /*
     * Bind kernel code and the kernel argument buffer to the
     * aql packet.
     */
    aql->kernel_object_address=hsaCodeDescriptor->code.handle;
    aql->kernarg_address=(uint64_t)kernel_arg_buffer;

    /*
     * Register the memory region for the argument buffer.
     */
    err = hsa_memory_register(args, sizeof(syrkKArgs));
    check(Registering the argument buffer, err);
}

void createSyrkAQLPacket(hsa_signal_t signal, hsa_dispatch_packet_t* aql, hsa_ext_code_descriptor_t* hsaCodeDescriptor, float* matrix, int n, int nb, int invocationID, int ordered) {
    syrkArgs thisSyrkArgs;
    int constFactor = nb/128;
    int gridSize = (2 * constFactor * constFactor + constFactor) * 64;
    setupSyrkAqlDispatchInfo(aql, signal, gridSize, ordered);
    initSyrkArgs(&thisSyrkArgs, n, nb, invocationID);
    syrkKArgs args;
    setupSyrkKernelArgs(&args, &thisSyrkArgs, matrix);
    loadSyrkKernelArgs(&args, aql, hsaCodeDescriptor);
}
