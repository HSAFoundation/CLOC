#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <assert.h>
#include "hsa.h"
#include "hsa_ext_finalize.h"
#include "elf_utils.h"

typedef uint32_t uint;

#define NSECPERSEC 1000000000

#define check(msg, status) \
if (status != HSA_STATUS_SUCCESS) { \
    printf("%s failed.\n", #msg); \
    exit(1); \
} else { \
   /*printf("%s succeeded.\n", #msg); */\
}

void createHSAProgram(char* fileName, char* kernel_name, hsa_ext_program_handle_t* hsaProgramPtr, hsa_ext_code_descriptor_t** hsaCodeDescriptor);
hsa_status_t find_symbol_offset(hsa_ext_brig_module_t* brig_module, char* symbol_name, hsa_ext_brig_code_section_offset32_t* offset);
void connectSignal(hsa_signal_t signal, hsa_dispatch_packet_t* aql);
hsa_status_t get_kernarg(hsa_region_t region, void* data);
void copyBlock(float* host, uint offsetHost, float* dest, uint offsetDest, uint ldh, uint ldd);
