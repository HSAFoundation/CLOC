#ifndef HSA_RUNTIME_CORE_INC_AMD_KERNEL_CODE_H_
#define HSA_RUNTIME_CORE_INC_AMD_KERNEL_CODE_H_

//#include <cstdint>
//#include "core/util/utils.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef uint32_t amd_kernel_code_version32_t;
typedef enum {
  AMD_KERNEL_CODE_VERSION_MAJOR = 1,
  AMD_KERNEL_CODE_VERSION_MINOR = 0
} amd_kernel_code_version_t;

typedef uint16_t amd_machine_kind16_t;
typedef uint16_t amd_machine_version16_t;
typedef enum {
  AMD_MACHINE_KIND_UNDEFINED = 0,
  AMD_MACHINE_KIND_AMDGPU = 1
} amd_machine_kind_t;

typedef enum {
  AMD_FLOAT_ROUND_TO_NEAREST_EVEN = 0,
  AMD_FLOAT_ROUND_TO_PLUS_INFINITY = 1,
  AMD_FLOAT_ROUND_TO_MINUS_INFINITY = 2,
  AMD_FLOAT_ROUND_TO_ZERO = 3
} amd_float_round_mode_t;

typedef enum {
  AMD_FLOAT_DENORM_FLUSH_SOURCE_OUTPUT = 0,
  AMD_FLOAT_DENORM_FLUSH_OUTPUT = 1,
  AMD_FLOAT_DENORM_FLUSH_SOURCE = 2,
  AMD_FLOAT_DENORM_NO_FLUSH = 3
} amd_float_denorm_mode_t;

typedef enum {
  AMD_SYSTEM_VGPR_WORKITEM_ID_X = 0,
  AMD_SYSTEM_VGPR_WORKITEM_ID_X_Y = 1,
  AMD_SYSTEM_VGPR_WORKITEM_ID_X_Y_Z = 2,
  AMD_SYSTEM_VGPR_WORKITEM_ID_UNDEFINED = 3
} amd_system_vgpr_workitem_id_t;

typedef enum {
  AMD_ELEMENT_2_BYTES = 0,
  AMD_ELEMENT_4_BYTES = 1,
  AMD_ELEMENT_8_BYTES = 2,
  AMD_ELEMENT_16_BYTES = 3
} amd_element_byte_size_t;

typedef uint8_t amd_powertwo8_t;
typedef enum {
  AMD_POWERTWO_1 = 0,
  AMD_POWERTWO_2 = 1,
  AMD_POWERTWO_4 = 2,
  AMD_POWERTWO_8 = 3,
  AMD_POWERTWO_16 = 4,
  AMD_POWERTWO_32 = 5,
  AMD_POWERTWO_64 = 6,
  AMD_POWERTWO_128 = 7,
  AMD_POWERTWO_256 = 8
} amd_powertwo_t;

typedef struct amd_compute_pgm_rsrc1_s {
  uint32_t granulated_workitem_vgpr_count :6;
  uint32_t granulated_wavefront_sgpr_count :4;
  uint32_t priority :2;
  uint32_t float_mode_round_32 :2;
  uint32_t float_mode_round_16_64 :2;
  uint32_t float_mode_denorm_32 :2;
  uint32_t float_mode_denorm_16_64 :2;
  uint32_t priv :1;
  uint32_t enable_dx10_clamp :1;
  uint32_t debug_mode :1;
  uint32_t enable_ieee_mode :1;
  uint32_t bulky :1;
  uint32_t cdbg_user :1;
  uint32_t reserved :6;
} amd_compute_pgm_rsrc1_t;

typedef struct amd_compute_pgm_rsrc2_s {
  uint32_t enable_sgpr_private_segment_wave_byte_offset :1;
  uint32_t user_sgpr_count :5;
  uint32_t enable_trap_handler :1;
  uint32_t enable_sgpr_workgroup_id_x :1;
  uint32_t enable_sgpr_workgroup_id_y :1;
  uint32_t enable_sgpr_workgroup_id_z :1;
  uint32_t enable_sgpr_workgroup_info :1;
  uint32_t enable_vgpr_workitem_id :2;
  uint32_t enable_exception_address_watch :1;
  uint32_t enable_exception_memory_violation :1;
  uint32_t granulated_lds_size :9;
  uint32_t enable_exception_ieee_754_fp_invalid_operation :1;
  uint32_t enable_exception_fp_denormal_source :1;
  uint32_t enable_exception_ieee_754_fp_division_by_zero :1;
  uint32_t enable_exception_ieee_754_fp_overflow :1;
  uint32_t enable_exception_ieee_754_fp_underflow :1;
  uint32_t enable_exception_ieee_754_fp_inexact :1;
  uint32_t enable_exception_int_divide_by_zero :1;
  uint32_t reserved :1;
} amd_compute_pgm_rsrc2_t;

typedef union amd_compute_pgm_resource_registers64_u {
  uint64_t raw;
  struct {
    amd_compute_pgm_rsrc1_t compute_pgm_rsrc1;
    amd_compute_pgm_rsrc2_t compute_pgm_rsrc2;
  };
} amd_compute_pgm_resource_registers64_t;

typedef uint16_t amd_exception_kind16_t;
typedef enum {
  AMD_EXCEPTION_INVALID_OPERATION = 1,
  AMD_EXCEPTION_DIVIDE_BY_ZERO = 2,
  AMD_EXCEPTION_OVERFLOW = 4,
  AMD_EXCEPTION_UNDERFLOW = 8,
  AMD_EXCEPTION_INEXACT = 16
} amd_exception_kind_t;


typedef uint64_t amd_enabled_control_directive64_t;
typedef enum {
  AMD_ENABLED_CONTROL_DIRECTIVE_ENABLE_BREAK_EXCEPTIONS = 0,
  AMD_ENABLED_CONTROL_DIRECTIVE_ENABLE_DETECT_EXCEPTIONS = 1,
  AMD_ENABLED_CONTROL_DIRECTIVE_MAX_DYNAMIC_GROUP_SIZE = 2,
  AMD_ENABLED_CONTROL_DIRECTIVE_MAX_FLAT_GRID_SIZE = 4,
  AMD_ENABLED_CONTROL_DIRECTIVE_MAX_FLAT_WORKGROUP_SIZE = 8,
  AMD_ENABLED_CONTROL_DIRECTIVE_REQUESTED_WORKGROUPS_PER_CU = 16,
  AMD_ENABLED_CONTROL_DIRECTIVE_REQUIRED_GRID_SIZE = 32,
  AMD_ENABLED_CONTROL_DIRECTIVE_REQUIRED_WORKGROUP_SIZE = 64,
  AMD_ENABLED_CONTROL_DIRECTIVE_REQUIRED_DIM = 128,
  AMD_ENABLED_CONTROL_DIRECTIVE_REQUIRE_NO_PARTIAL_WORKGROUPS = 256
} amd_enabled_control_directive_t;

typedef struct amd_control_directives_s {
  amd_enabled_control_directive64_t enabled_control_directives;
  uint16_t enable_break_exceptions;
  uint16_t enable_detect_exceptions;
  uint32_t max_dynamic_group_size;
  uint64_t max_flat_grid_size;
  uint32_t max_flat_workgroup_size;
  uint32_t reserved1;
  uint64_t required_grid_size[3];
  uint32_t required_workgroup_size[3];
  uint8_t required_dim;
  uint8_t reserved2[59];
} __attribute__((__aligned__(64))) amd_control_directives_t;

typedef struct amd_kernel_code_s {
  amd_kernel_code_version32_t amd_kernel_code_version_major;
  amd_kernel_code_version32_t amd_kernel_code_version_minor;
  amd_machine_kind16_t amd_machine_kind;
  amd_machine_version16_t amd_machine_version_major;
  amd_machine_version16_t amd_machine_version_minor;
  amd_machine_version16_t amd_machine_version_stepping;
  int64_t kernel_code_entry_byte_offset;
  int64_t kernel_code_prefetch_byte_offset;
  uint64_t kernel_code_prefetch_byte_size;
  uint64_t max_scratch_backing_memory_byte_size;
  amd_compute_pgm_resource_registers64_t compute_pgm_resource_registers;
  uint32_t enable_sgpr_private_segment_buffer :1;
  uint32_t enable_sgpr_dispatch_ptr :1;
  uint32_t enable_sgpr_queue_ptr :1;
  uint32_t enable_sgpr_kernarg_segment_ptr :1;
  uint32_t enable_sgpr_dispatch_id :1;
  uint32_t enable_sgpr_flat_scratch_init :1;
  uint32_t enable_sgpr_private_segment_size :1;
  uint32_t enable_sgpr_grid_workgroup_count_x :1;
  uint32_t enable_sgpr_grid_workgroup_count_y :1;
  uint32_t enable_sgpr_grid_workgroup_count_z :1;
  uint32_t reserved1 :6;
  uint32_t enable_ordered_append_gds :1;
  uint32_t private_element_size :2;
  uint32_t is_ptr64 :1;
  uint32_t is_dynamic_callstack :1;
  uint32_t is_debug_enabled :1;
  uint32_t is_xnack_enabled :1;
  uint32_t reserved2 :9;
  uint32_t workitem_private_segment_byte_size;
  uint32_t workgroup_group_segment_byte_size;
  uint32_t gds_segment_byte_size;
  uint64_t kernarg_segment_byte_size;
  uint32_t workgroup_fbarrier_count;
  uint16_t wavefront_sgpr_count;
  uint16_t workitem_vgpr_count;
  uint16_t reserved_vgpr_first;
  uint16_t reserved_vgpr_count;
  uint16_t reserved_sgpr_first;
  uint16_t reserved_sgpr_count;
  uint16_t debug_wavefront_private_segment_offset_sgpr;
  uint16_t debug_private_segment_buffer_sgpr;
  amd_powertwo8_t kernarg_segment_alignment;
  amd_powertwo8_t group_segment_alignment;
  amd_powertwo8_t private_segment_alignment;
  amd_powertwo8_t wavefront_size;
  uint64_t reserved4[2];
  uint64_t runtime_loader_kernel_symbol;
  amd_control_directives_t control_directives;
} __attribute__((__aligned__(64))) amd_kernel_code_t;

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif // HSA_RUNTIME_CORE_INC_AMD_KERNEL_CODE_H_
