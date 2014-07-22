#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "hsa.h"
#include "hsa_ext_finalize.h"
#include "hsa_ext_private_amd.h"
#include "assemble.h"
#include "elf_utils.hpp"

#define MULTILINE(...) # __VA_ARGS__

#if defined(_MSC_VER)
#define ALIGNED_(x) __declspec(align(x))
#else
#if defined(__GNUC__)
#define ALIGNED_(x) __attribute__ ((aligned(x)))
#endif
#endif

inline void ErrorCheck(hsa_status_t err)
{
	if(err!=HSA_STATUS_SUCCESS)
	{
		printf("HSA reported error!\n");
		exit(0);
	}
}

static hsa_status_t IterateAgent(hsa_agent_t agent, void *data) {
	// Find GPU device and use it.
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

static hsa_status_t IterateRegion(hsa_region_t region, void *data) {
	// Find system memory region.
		if (data == NULL) {
			return HSA_STATUS_ERROR_INVALID_ARGUMENT;
		}

		bool is_host = false;
		hsa_status_t stat =
			hsa_region_get_info(
			region, (hsa_region_info_t)HSA_EXT_REGION_INFO_HOST_ACCESS, &is_host);
		if (stat != HSA_STATUS_SUCCESS) {
			return stat;
		}

		if (is_host) {
			*((hsa_region_t *)data) = region;
		}
		return HSA_STATUS_SUCCESS;
	}

int main(int argc, char **argv)
{
	
	hsa_status_t err;

	err=hsa_init();
	ErrorCheck(err);

	//Get GPU device
	hsa_agent_t device = 0;
	err = hsa_iterate_agents(IterateAgent, &device);
	ErrorCheck(err);

	if(device == 0)
	{
		printf("No HSA devices found!\n");
		return 1;
	}

	//Print out name of the device
	char name[64] = { 0 };
	err = hsa_agent_get_info(device, HSA_AGENT_INFO_NAME, name);
	ErrorCheck(err);
	printf("Using: %s\n", name);

	//Get queue size
	uint32_t queue_size = 0;
	err = hsa_agent_get_info(device, HSA_AGENT_INFO_QUEUE_MAX_SIZE, &queue_size);
	ErrorCheck(err);

	hsa_queue_t* commandQueue;
	err =
		hsa_queue_create(
		device, queue_size, HSA_QUEUE_TYPE_MULTI, NULL, NULL, &commandQueue);
	ErrorCheck(err);

	//Convert hsail kernel text to BRIG.
	hsa_ext_brig_module_t* brigModule;
    std::string file_name("vector_copy.brig");
	if (!CreateBrigModuleFromBrigFile(file_name.c_str(), &brigModule)){
		ErrorCheck(HSA_STATUS_ERROR);
	}

	//Create hsa program.
	hsa_ext_program_handle_t hsaProgram;
	err = hsa_ext_program_create(&device, 1, HSA_EXT_BRIG_MACHINE_LARGE, HSA_EXT_BRIG_PROFILE_FULL, &hsaProgram);
	ErrorCheck(err);

	//Add BRIG module to hsa program.
	hsa_ext_brig_module_handle_t module;
	err = hsa_ext_add_module(hsaProgram, brigModule, &module);
	ErrorCheck(err);

	// Construct finalization request list.
	// @todo kzhuravl 6/16/2014 remove bare numbers, we actually need to find
	// entry offset into the code section.
	hsa_ext_finalization_request_t finalization_request_list;
	finalization_request_list.module = module;              // module handle.
	finalization_request_list.symbol = 192;                 // entry offset into the code section.
	finalization_request_list.program_call_convention = 0;  // program call convention. not supported.

	if (!FindSymbolOffset(brigModule, "&__OpenCL_test_kernel", KERNEL_SYMBOLS, finalization_request_list.symbol)){
		ErrorCheck(HSA_STATUS_ERROR);
	}

	//Finalize hsa program.
	err = hsa_ext_finalize_program(hsaProgram, device, 1, &finalization_request_list, NULL, NULL, 0, NULL, 0);
	ErrorCheck(err);

	//Get hsa code descriptor address.
	hsa_ext_code_descriptor_t *hsaCodeDescriptor;
	err = hsa_ext_query_kernel_descriptor_address(hsaProgram, module, finalization_request_list.symbol, &hsaCodeDescriptor);
	ErrorCheck(err);

	//Get a signal
	hsa_signal_t signal;
	err=hsa_signal_create(1, 0, NULL, &signal);
	ErrorCheck(err);

	//setup dispatch packet
	hsa_dispatch_packet_t aql;
	memset(&aql, 0, sizeof(aql));

	//Setup dispatch size and fences
	aql.completion_signal=signal;
	aql.dimensions=1;
	aql.workgroup_size_x=256;
	aql.workgroup_size_y=1;
	aql.workgroup_size_z=1;
	aql.grid_size_x=1024*1024;
	aql.grid_size_y=1;
	aql.grid_size_z=1;
	aql.header.type=HSA_PACKET_TYPE_DISPATCH;
	aql.header.acquire_fence_scope=2;
	aql.header.release_fence_scope=2;
	aql.header.barrier=1;
	aql.group_segment_size=0;
	aql.private_segment_size=0;
	
	//Setup kernel arguments
	char* in=(char*)malloc(1024*1024*4);
	char* out=(char*)malloc(1024*1024*4);

	//printf("%p\n", in);
	//printf("%p\n", out);

	memset(out, 0, 1024*1024*4);
	memset(in, 1, 1024*1024*4);

	err=hsa_memory_register(in, 1024*1024*4);
	ErrorCheck(err);
	err=hsa_memory_register(out, 1024*1024*4);
	ErrorCheck(err);

	//hsaKernelArgs<void(uint, uint, uint, void*, void*, uint64)> args;
	struct ALIGNED_(HSA_ARGUMENT_ALIGN_BYTES) args_t
	{
		uint64_t arg0;
		uint64_t arg1;
		uint64_t arg2;
		void* arg3;
		void* arg4;
		uint32_t arg5;
	} args;
	args.arg0=0;
	args.arg1=0;
	args.arg2=0;
	args.arg3=out;
	args.arg4=in;
	args.arg5=1024*1024;
	
	//Bind kernel arguments and kernel code
	aql.kernel_object_address=hsaCodeDescriptor->code.handle;
	aql.kernarg_address=(uint64_t)&args;

	//Register argument buffer
	hsa_memory_register(&args, sizeof(args_t));

	const uint32_t queueSize=commandQueue->size;
	const uint32_t queueMask=queueSize-1;

	//write to command queue
	//for(int i=0; i<1000; i++)
	{
		uint64_t index=hsa_queue_load_write_index_relaxed(commandQueue);
		((hsa_dispatch_packet_t*)(commandQueue->base_address))[index&queueMask]=aql;
		hsa_queue_store_write_index_relaxed(commandQueue, index+1);

		//Ringdoor bell
		hsa_signal_store_relaxed(commandQueue->doorbell_signal, index+1);

        if (hsa_signal_wait_acquire(signal, HSA_LT, 1, uint64_t(-1), HSA_WAIT_EXPECTANCY_UNKNOWN)!=0)
		{
			printf("Signal wait returned unexpected value\n");
			exit(0);
		}

		hsa_signal_store_relaxed(signal, 1);
	}

	//Validate
	bool valid=true;
	int failIndex=0;
	for(int i=0; i<1024*1024; i++)
	{
		if(out[i]!=in[i])
		{
			failIndex=i;
			valid=false;
			break;
		}
	}
	if(valid)
		printf("passed validation\n");
	else
	{
		printf("VALIDATION FAILED!\nBad index: %d\n", failIndex);
	}

	//Cleanup
	err=hsa_signal_destroy(signal);
	ErrorCheck(err);

	err=hsa_ext_program_destroy(hsaProgram);
	ErrorCheck(err);

	err=hsa_queue_destroy(commandQueue);
	ErrorCheck(err);
	
	err=hsa_shut_down();
	ErrorCheck(err);

	free(in);
	free(out);

    return 0;

}

