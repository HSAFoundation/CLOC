/* Copyright 2014 HSA Foundation Inc.  All Rights Reserved.
 *
 * HSAF is granting you permission to use this software and documentation (if
 * any) (collectively, the "Materials") pursuant to the terms and conditions
 * of the Software License Agreement included with the Materials.  If you do
 * not have a copy of the Software License Agreement, contact the  HSA Foundation for a copy.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS WITH THE SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "magmaWrapper.h"
#include "lapacke.h"
#include "choleskyTile.h"
#include "spotrf_hsa.h"
#include "hsaPortCholeskyImpl.h"
#include "spotrf_task.h"

static long int get_nanosecs(struct timespec start, struct timespec end) {
    return (end.tv_nsec - start.tv_nsec);
}

/*
 * Define required BRIG data structures.
 */

typedef uint32_t BrigCodeOffset32_t;

typedef uint32_t BrigDataOffset32_t;

typedef uint16_t BrigKinds16_t;

typedef uint8_t BrigLinkage8_t;

typedef uint8_t BrigExecutableModifier8_t;

typedef BrigDataOffset32_t BrigDataOffsetString32_t;

enum BrigKinds {
    BRIG_KIND_NONE = 0x0000,
    BRIG_KIND_DIRECTIVE_BEGIN = 0x1000,
    BRIG_KIND_DIRECTIVE_KERNEL = 0x1008,
};

typedef struct BrigBase BrigBase;
struct BrigBase {
    uint16_t byteCount;
    BrigKinds16_t kind;
};

typedef struct BrigExecutableModifier BrigExecutableModifier;
struct BrigExecutableModifier {
    BrigExecutableModifier8_t allBits;
};

typedef struct BrigDirectiveExecutable BrigDirectiveExecutable;
struct BrigDirectiveExecutable {
    uint16_t byteCount;
    BrigKinds16_t kind;
    BrigDataOffsetString32_t name;
    uint16_t outArgCount;
    uint16_t inArgCount;
    BrigCodeOffset32_t firstInArg;
    BrigCodeOffset32_t firstCodeBlockEntry;
    BrigCodeOffset32_t nextModuleEntry;
    uint32_t codeBlockEntryCount;
    BrigExecutableModifier modifier;
    BrigLinkage8_t linkage;
    uint16_t reserved;
};

typedef struct BrigData BrigData;
struct BrigData {
    uint32_t byteCount;
    uint8_t bytes[1];
};

/*
 * Determines if the given agent is of type HSA_DEVICE_TYPE_GPU
 * and sets the value of data to the agent handle if it is.
 */
static hsa_status_t find_gpu(hsa_agent_t agent, void *data) {
    if (data == NULL) {
        return HSA_STATUS_ERROR_INVALID_ARGUMENT;
    }
    hsa_device_type_t device_type;
    hsa_status_t stat =
    hsa_agent_get_info(agent, HSA_AGENT_INFO_DEVICE, &device_type);
    if (stat != HSA_STATUS_SUCCESS) {
        return stat;
    }
    if (device_type == HSA_DEVICE_TYPE_GPU) {
        *((hsa_agent_t *)data) = agent;
    }
    return HSA_STATUS_SUCCESS;
}

/*
 * Finds the specified symbols offset in the specified brig_module.
 * If the symbol is found the function returns HSA_STATUS_SUCCESS, 
 * otherwise it returns HSA_STATUS_ERROR.
 */
//hsa_status_t find_symbol_offset(hsa_ext_brig_module_t* brig_module, 
//    char* symbol_name,
//    hsa_ext_brig_code_section_offset32_t* offset) {
//    
//    /* 
//     * Get the data section 
//     */
//    hsa_ext_brig_section_header_t* data_section_header = 
//                brig_module->section[HSA_EXT_BRIG_SECTION_DATA];
//    /* 
//     * Get the code section
//     */
//    hsa_ext_brig_section_header_t* code_section_header =
//             brig_module->section[HSA_EXT_BRIG_SECTION_CODE];
//
//    /* 
//     * First entry into the BRIG code section
//     */
//    BrigCodeOffset32_t code_offset = code_section_header->header_byte_count;
//    BrigBase* code_entry = (BrigBase*) ((char*)code_section_header + code_offset);
//    while (code_offset != code_section_header->byte_count) {
//        if (code_entry->kind == BRIG_KIND_DIRECTIVE_KERNEL) {
//            /* 
//             * Now find the data in the data section
//             */
//            BrigDirectiveExecutable* directive_kernel = (BrigDirectiveExecutable*) (code_entry);
//            BrigDataOffsetString32_t data_name_offset = directive_kernel->name;
//            BrigData* data_entry = (BrigData*)((char*) data_section_header + data_name_offset);
//            if (!strncmp(symbol_name, (char*) data_entry->bytes, strlen(symbol_name))) {
//                *offset = code_offset;
//                return HSA_STATUS_SUCCESS;
//            }
//        }
//        code_offset += code_entry->byteCount;
//        code_entry = (BrigBase*) ((char*)code_section_header + code_offset);
//    }
//    return HSA_STATUS_ERROR;
//}

extern hsa_agent_t device;
hsa_queue_t* taskingCommandQueue[10];
int numQueues = 10;
hsa_ext_program_handle_t taskingSyrkProgram;
hsa_ext_code_descriptor_t *taskingSyrkCodeDescriptor;
hsa_ext_program_handle_t taskingGemmProgram;
hsa_ext_code_descriptor_t *taskingGemmCodeDescriptor;
hsa_ext_program_handle_t taskingTrsmProgram;
hsa_ext_code_descriptor_t *taskingTrsmCodeDescriptor;
hsa_dispatch_packet_t taskingAQLGemm[10];
hsa_dispatch_packet_t taskingAQLSyrk[10];
hsa_dispatch_packet_t taskingAQLTrsm[10];
hsa_status_t taskingErr;
int initialized = 0;
real_Double_t gpu_time;
magma_int_t
taskingCholeskyImpl(
    magma_uplo_t   uplo, magma_int_t    n,
    float* dA, size_t dA_offset, magma_int_t ldda,
    magma_int_t*   info, int lastIter )
{

    taskingErr = hsa_init();
    check(Initializing the hsa runtime, taskingErr);

    /* 
     * Iterate over the agents and pick the gpu agent using 
     * the find_gpu callback.
     */
    taskingErr = hsa_iterate_agents(find_gpu, &device);
    check(Calling hsa_iterate_agents, taskingErr);

    taskingErr = (device == 0) ? HSA_STATUS_ERROR : HSA_STATUS_SUCCESS;
    check(Checking if the GPU device is non-zero, taskingErr);

    /*
     * Query the name of the device.
     */
    char name[64] = { 0 };
    taskingErr = hsa_agent_get_info(device, HSA_AGENT_INFO_NAME, name);
    check(Querying the device name, taskingErr);
    printf("The device name is %s.\n", name);

    /*
     * Query the maximum size of the queue.
     */
    uint32_t queue_size = 0;
    taskingErr = hsa_agent_get_info(device, HSA_AGENT_INFO_QUEUE_MAX_SIZE, &queue_size);
    check(Querying the device maximum queue size, taskingErr);
    printf("The maximum queue size is %u.\n", (unsigned int) queue_size);

    /*
     * Create a queue using the maximum size.
     */
    int queueIter;
    for (queueIter = 0; queueIter < numQueues; queueIter++) {
        taskingErr = hsa_queue_create(device, queue_size, HSA_QUEUE_TYPE_MULTI, NULL, NULL, &taskingCommandQueue[queueIter]);
        check(Creating the queue, taskingErr);
    }
    /*
     * Create program
     */
    ////////************** Step 1: Create syrk kernel *************************////////
    char syrk_file_name[128] = "syrkKernel.brig";
    char syrk_kernel_name[128] = "&__OpenCL_ssyrkBlock_kernel";
    if (initialized == 0) {
        createHSAProgram(syrk_file_name, syrk_kernel_name, &taskingSyrkProgram, &taskingSyrkCodeDescriptor);
    }

    ////////************** Step 2: Create gemm kernel *************************////////
    char gemm_file_name[128] = "gemmKernel.brig";
    char gemm_kernel_name[128] = "&__OpenCL_sgemmBlock_kernel";
    if (initialized == 0) {
        createHSAProgram(gemm_file_name, gemm_kernel_name, &taskingGemmProgram, &taskingGemmCodeDescriptor);
    }

    ////////************** Step 3: Create trsm kernel *************************////////
    char trsm_file_name[128] = "trsmKernel.brig";
    char trsm_kernel_name[128] = "&__OpenCL_strsmCached_kernel";
    if (initialized == 0) {
        createHSAProgram(trsm_file_name, trsm_kernel_name, &taskingTrsmProgram, &taskingTrsmCodeDescriptor);
    }
    /*
     * Initialize the dispatch packet.
     */
    int i;
    for (i = 0; i < 10; i++) {
        memset(&taskingAQLGemm[i], 0, sizeof(taskingAQLGemm[i]));
        memset(&taskingAQLSyrk[i], 0, sizeof(taskingAQLSyrk[i]));
        memset(&taskingAQLTrsm[i], 0, sizeof(taskingAQLTrsm[i]));
    }


//    LAPACK_spotrf( (char*)lapack_uplo_const(uplo), &n, dA, &ldda, info );
    real_Double_t timeThisIter;
    timeThisIter = magma_wtime();
    spotrf_task(uplo, n, dA, dA_offset, ldda, info);
    timeThisIter = magma_wtime() - timeThisIter;
    if (initialized == 0) {
        gpu_time = timeThisIter;
    } else if (timeThisIter < gpu_time) {
        gpu_time = timeThisIter;
    }
    initialized = 1;
    printf("TASK:  (%7.6f)\n",
           gpu_time);
    /*
     * Cleanup all allocated resources.
     */

    //taskingErr=hsa_ext_program_destroy(taskingSyrkProgram);
    //check(Destroying the program, taskingErr);

    //taskingErr=hsa_ext_program_destroy(taskingGemmProgram);
    //check(Destroying the program, taskingErr);

    //taskingErr=hsa_ext_program_destroy(taskingTrsmProgram);
    //check(Destroying the program, taskingErr);

    for (queueIter = 0; queueIter < numQueues; queueIter++) {
        taskingErr=hsa_queue_destroy(taskingCommandQueue[queueIter]);
        check(Destroying the queue, taskingErr);
    }
       
    taskingErr=hsa_shut_down();
    check(Shutting down the runtime, taskingErr);
    return 0;
}
