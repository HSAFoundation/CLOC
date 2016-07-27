#include <clc/clc.h>
/* 
   Gets memory contents pointed to by a reserved field of the dispatch packet
   dispatch_p->reserved_device_mem_ptr is a pointer to the aligned 128-byte 
   reserved memory area on the device that is initialized on the host.
   That area can be treated as 16 64-bit values or pointers
   That area can be treated as 32 32-bit values 
   That area can be treated as 64 16-bit values 
*/

/* kernel dispatch packet */
typedef struct kernel_dispatch_packet_s {
  uint16_t header;               /*  0 32  0 64 */
  uint16_t setup;
  uint16_t workgroup_size_x;     /* 1 32 */
  uint16_t workgroup_size_y;
  uint16_t workgroup_size_z;     /* 2 32   1 64 */
  uint16_t reserved0;
  uint32_t grid_size_x;          /* 3 32 */
  uint32_t grid_size_y;          /* 4 32   2 64 */
  uint32_t grid_size_z;          /* 5 32 */ 
  uint32_t private_segment_size; /* 6 32   3 64 */
  uint32_t group_segment_size;   /* 7 32 */
  uint64_t kernel_object;        /* 8 32      4 64 */
  uint64_t kernarg_address;      /* 10 32     5 64 */
  uint64_t reserved_device_mem_ptr; /* 12 32  6 64  48 8*/
  uint64_t completion_signal;
} kernel_dispatch_packet_t;

_CLC_DEF void* get_reserved_device_mem(uint index) { 
   /* index is 0-15 or 0-31 depending on size of pointer  */
   kernel_dispatch_packet_t * dispatch_p = (kernel_dispatch_packet_t*) get_dispatch_ptr();
   return( (void*) ( &dispatch_p->reserved_device_mem_ptr + (sizeof(void*)*index )) ); 
}

_CLC_DEF uint64_t get_reserved_device_mem_64(uint index) { 
   /* index is 0-15 */
   kernel_dispatch_packet_t * dispatch_p = (kernel_dispatch_packet_t*) get_dispatch_ptr();
   return( (uint64_t) ( &dispatch_p->reserved_device_mem_ptr + (sizeof(uint64_t)*index )) ); 
}

_CLC_DEF uint32_t get_reserved_device_mem_32(uint index) { 
   /* index is 0-31 */
   kernel_dispatch_packet_t * dispatch_p = (kernel_dispatch_packet_t*) get_dispatch_ptr();
   return( (uint32_t) ( &dispatch_p->reserved_device_mem_ptr + (sizeof(uint32_t)*index )) ); 
}

