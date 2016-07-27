
#ifndef _OMPTARGET_AMDGCN_OPTION_H_
typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long int64_t;
typedef unsigned long uint64_t;
#endif

/*  get_reserved_device_mem_ptr returns pointer to the 128byte reserved memory area */
_CLC_DEF __global void* get_reserved_device_mem_ptr(); 
/*  the 128byte reserved memory area holds 16 64-bit global pointers */
/*  get_reserved_device_mem gets a particular 64-bit pointer specified by the index argument */
_CLC_DEF __global void* get_reserved_device_mem(uint index); 
_CLC_DEF uint64_t get_reserved_device_mem_64(uint index); 
_CLC_DEF uint32_t get_reserved_device_mem_32(uint index);
