#include "choleskyTile.h"

extern hsa_agent_t device;
extern hsa_status_t err;

void createHSAProgram(char* file_name, char* kernel_name, hsa_ext_program_handle_t* hsaProgramPtr, hsa_ext_code_descriptor_t** hsaCodeDescriptor) {
    /*
     * Load BRIG, encapsulated in an ELF container, into a BRIG module.
     */
    hsa_ext_brig_module_t* brigModule;
    err = create_brig_module_from_brig_file(file_name, &brigModule);
    check(Creating the brig module from vector_add.brig, err);

    /*
     * Create hsa program.
     */
    err = hsa_ext_program_create(&device, 1, HSA_EXT_BRIG_MACHINE_LARGE, HSA_EXT_BRIG_PROFILE_FULL, hsaProgramPtr);
    check(Creating the hsa program, err);

    /*
     * Add the BRIG module to hsa program.
     */
    hsa_ext_program_handle_t hsaProgram = *(hsaProgramPtr);
    hsa_ext_brig_module_handle_t module;
    err = hsa_ext_add_module(hsaProgram, brigModule, &module);
    check(Adding the brig module to the program, err);

    /* 
     * Construct finalization request list.
     */
    hsa_ext_finalization_request_t finalization_request_list;
    finalization_request_list.module = module;
    finalization_request_list.program_call_convention = 0;
    err = find_symbol_offset(brigModule, kernel_name, &finalization_request_list.symbol);
    check(Finding the symbol offset for the kernel, err);

    /*
     * Finalize the hsa program.
     */
    err = hsa_ext_finalize_program(hsaProgram, device, 1, &finalization_request_list, NULL, NULL, 0, NULL, 0);
    check(Finalizing the program, err);

    /*
     * Destroy the brig module. The program was successfully created the kernel
     * symbol was found and the program was finalized, so it is no longer needed.
     */
     destroy_brig_module(brigModule);

    /*
     * Get the hsa code descriptor address.
     */
    err = hsa_ext_query_kernel_descriptor_address(hsaProgram, module, finalization_request_list.symbol, hsaCodeDescriptor);
    check(Querying the kernel descriptor address, err);
}
