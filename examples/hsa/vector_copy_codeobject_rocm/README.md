vector_copy_codeobject sample modified to run on ROCM platform , 
so the code can run on either APU or dGPU.  The changes include:

- use kernarg region for kernel arguments.
- find a device memory region and use this to allocate in_d and out_d memory regions.
  Also add commands to explicitly copy the in->in_d before running kernel, and
  copy out_d -> out after kernel finishes.
- remove memory registration code.
