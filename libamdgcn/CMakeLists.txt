##===----------------------------------------------------------------------===##
# 
#                     The LLVM Compiler Infrastructure
#
# This file is dual licensed under the MIT and the University of Illinois Open
# Source Licenses. See LICENSE.txt for details.
##===----------------------------------------------------------------------===##
#
# libamdgcn/CMakeLists.txt
# This cmake replaces the old make version in hidden directory .old_make_method
# Written by Greg Rodgers (Gregory.Rodgers@amd.com)
#
##===----------------------------------------------------------------------===##
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project (LIBAMDGCN)
add_custom_target(LIBAMDGCN ALL) 
set(llvm /opt/amd/llvm)
set(optimization_level 3)

set(amdgcn_mcpus fiji kaveri carrizo)
#set(amdgcn_mcpus kaveri)

set(cl_cc_cmd ${llvm}/bin/clang -c -target amdgcn--amdhsa -x cl -Xclang -cl-std=CL2.0 -Ucl_khr_fp64 -Ucl_khr_byte_addressable_store -Dcl_clang_storage_class_specifiers -Dcles_khr_int64 -D__CLC_INTERNAL -emit-llvm -I${CMAKE_SOURCE_DIR}/libclc_overrides/include -I${CMAKE_SOURCE_DIR}/libclc/generic/include -fno-builtin )

# These are the ll files that will be built
set(amdgcn_llfiles
         libclc/amdgcn/lib/synchronization/barrier_impl.ll
         libclc/amdgpu/lib/image/get_image_attributes_impl.ll
         libclc/amdgpu/lib/image/read_image_impl.ll
         libclc/amdgpu/lib/image/write_image_impl.ll
         libclc/generic/lib/subnormal_helper_func.ll
         libclc/generic/lib/subnormal_use_default.ll
         libclc/generic/lib/atomic/atomic_impl.ll
         libclc/generic/lib/integer/add_sat_if.ll
         libclc/generic/lib/integer/add_sat_impl.ll
         libclc/generic/lib/integer/clz_if.ll
         libclc/generic/lib/integer/clz_impl.ll
         libclc/generic/lib/integer/sub_sat_if.ll
         libclc/generic/lib/integer/sub_sat_impl.ll
#         libclc_overrides/lib/workitem/get_dispatch_ptr.ll
         libclc_overrides/lib/workitem/get_global_size.ll
         libclc_overrides/lib/workitem/get_group_id_ll.ll
         libclc_overrides/lib/workitem/get_local_id_ll.ll
         libclc_overrides/lib/workitem/get_local_size.ll
#         libclc_overrides/lib/workitem/get_reserved_device_mem.ll
         libclc_overrides/lib/workitem/get_work_dim_ll.ll
)

# These are the cl files that will be built 
set(amdgcn_clfiles 
         libclc_overrides/lib/workitem/get_global_id.cl
         libclc_overrides/lib/workitem/get_local_id.cl
         libclc_overrides/lib/workitem/get_group_id.cl
         libclc_overrides/lib/workitem/get_num_groups.cl
         libclc_overrides/lib/workitem/get_work_dim.cl
         libclc/amdgpu/lib/synchronization/barrier.cl
         libclc/amdgpu/lib/atomic/atomic.cl
         libclc/amdgpu/lib/math/ldexp.cl
         libclc/amdgpu/lib/math/nextafter.cl
         libclc/amdgpu/lib/math/sqrt.cl
         libclc/amdgpu/lib/image/get_image_width.cl
         libclc/amdgpu/lib/image/get_image_height.cl
         libclc/amdgpu/lib/image/get_image_depth.cl
         libclc/amdgpu/lib/image/get_image_channel_data_type.cl
         libclc/amdgpu/lib/image/get_image_channel_order.cl
         libclc/amdgpu/lib/image/read_imagef.cl
         libclc/amdgpu/lib/image/read_imagei.cl
         libclc/amdgpu/lib/image/read_imageui.cl
         libclc/amdgpu/lib/image/write_imagef.cl
         libclc/amdgpu/lib/image/write_imagei.cl
         libclc/amdgpu/lib/image/write_imageui.cl
         libclc/generic/lib/subnormal_config.cl
         libclc/generic/lib/async/async_work_group_copy.cl
         libclc/generic/lib/async/async_work_group_strided_copy.cl
         libclc/generic/lib/async/prefetch.cl
         libclc/generic/lib/async/wait_group_events.cl
         libclc/generic/lib/atomic/atomic_xchg.cl
         libclc/generic/lib/cl_khr_global_int32_base_atomics/atom_add.cl
         libclc/generic/lib/cl_khr_global_int32_base_atomics/atom_cmpxchg.cl
         libclc/generic/lib/cl_khr_global_int32_base_atomics/atom_dec.cl
         libclc/generic/lib/cl_khr_global_int32_base_atomics/atom_inc.cl
         libclc/generic/lib/cl_khr_global_int32_base_atomics/atom_sub.cl
         libclc/generic/lib/cl_khr_global_int32_base_atomics/atom_xchg.cl
         libclc/generic/lib/cl_khr_global_int32_extended_atomics/atom_and.cl
         libclc/generic/lib/cl_khr_global_int32_extended_atomics/atom_max.cl
         libclc/generic/lib/cl_khr_global_int32_extended_atomics/atom_min.cl
         libclc/generic/lib/cl_khr_global_int32_extended_atomics/atom_or.cl
         libclc/generic/lib/cl_khr_global_int32_extended_atomics/atom_xor.cl
         libclc/generic/lib/cl_khr_local_int32_base_atomics/atom_add.cl
         libclc/generic/lib/cl_khr_local_int32_base_atomics/atom_cmpxchg.cl
         libclc/generic/lib/cl_khr_local_int32_base_atomics/atom_dec.cl
         libclc/generic/lib/cl_khr_local_int32_base_atomics/atom_inc.cl
         libclc/generic/lib/cl_khr_local_int32_base_atomics/atom_sub.cl
         libclc/generic/lib/cl_khr_local_int32_base_atomics/atom_xchg.cl
         libclc/generic/lib/cl_khr_local_int32_extended_atomics/atom_and.cl
         libclc/generic/lib/cl_khr_local_int32_extended_atomics/atom_max.cl
         libclc/generic/lib/cl_khr_local_int32_extended_atomics/atom_min.cl
         libclc/generic/lib/cl_khr_local_int32_extended_atomics/atom_or.cl
         libclc/generic/lib/cl_khr_local_int32_extended_atomics/atom_xor.cl
         libclc/generic/lib/common/degrees.cl
         libclc/generic/lib/common/mix.cl
         libclc/generic/lib/common/radians.cl
         libclc/generic/lib/common/sign.cl
         libclc/generic/lib/common/smoothstep.cl
         libclc/generic/lib/common/step.cl
         libclc/generic/lib/geometric/cross.cl
         libclc/generic/lib/geometric/distance.cl
         libclc/generic/lib/geometric/dot.cl
         libclc/generic/lib/geometric/fast_distance.cl
         libclc/generic/lib/geometric/fast_length.cl
         libclc/generic/lib/geometric/fast_normalize.cl
         libclc/generic/lib/geometric/length.cl
         libclc/generic/lib/geometric/normalize.cl
         libclc/generic/lib/integer/abs.cl
         libclc/generic/lib/integer/abs_diff.cl
         libclc/generic/lib/integer/add_sat.cl
         libclc/generic/lib/integer/clz.cl
         libclc/generic/lib/integer/hadd.cl
         libclc/generic/lib/integer/mad24.cl
         libclc/generic/lib/integer/mad_sat.cl
         libclc/generic/lib/integer/mul24.cl
         libclc/generic/lib/integer/mul_hi.cl
         libclc/generic/lib/integer/rhadd.cl
         libclc/generic/lib/integer/rotate.cl
         libclc/generic/lib/integer/sub_sat.cl
         libclc/generic/lib/integer/upsample.cl
         libclc/generic/lib/math/acos.cl
         libclc/generic/lib/math/acosh.cl
         libclc/generic/lib/math/acospi.cl
         libclc/generic/lib/math/asin.cl
         libclc/generic/lib/math/asinh.cl
         libclc/generic/lib/math/asinpi.cl
         libclc/generic/lib/math/atan.cl
         libclc/generic/lib/math/atan2.cl
         libclc/generic/lib/math/atan2pi.cl
         libclc/generic/lib/math/atanh.cl
         libclc/generic/lib/math/atanpi.cl
         libclc/generic/lib/math/copysign.cl
         libclc/generic/lib/math/cos.cl
         libclc/generic/lib/math/cospi.cl
         libclc/generic/lib/math/ep_log.cl
         libclc/generic/lib/math/erfc.cl
         libclc/generic/lib/math/exp.cl
         libclc/generic/lib/math/exp_helper.cl
         libclc/generic/lib/math/exp2.cl
         libclc/generic/lib/math/exp10.cl
         libclc/generic/lib/math/fmax.cl
         libclc/generic/lib/math/fmin.cl
         libclc/generic/lib/math/fmod.cl
         libclc/generic/lib/math/fract.cl
         libclc/generic/lib/math/frexp.cl
         libclc/generic/lib/math/half_rsqrt.cl
         libclc/generic/lib/math/half_sqrt.cl
         libclc/generic/lib/math/hypot.cl
         libclc/generic/lib/math/clc_ldexp.cl
         libclc/generic/lib/math/log.cl
         libclc/generic/lib/math/log10.cl
         libclc/generic/lib/math/log1p.cl
         libclc/generic/lib/math/log2.cl
         libclc/generic/lib/math/mad.cl
         libclc/generic/lib/math/modf.cl
         libclc/generic/lib/math/native_log.cl
         libclc/generic/lib/math/native_log2.cl
         libclc/generic/lib/math/tables.cl
         libclc/generic/lib/math/clc_nextafter.cl
         libclc/generic/lib/math/pown.cl
         libclc/generic/lib/math/sin.cl
         libclc/generic/lib/math/sincos.cl
         libclc/generic/lib/math/sincos_helpers.cl
         libclc/generic/lib/math/sinpi.cl
         libclc/generic/lib/math/clc_sqrt.cl
         libclc/generic/lib/math/tan.cl
         libclc/generic/lib/math/tanh.cl
         libclc/generic/lib/relational/all.cl
         libclc/generic/lib/relational/any.cl
         libclc/generic/lib/relational/bitselect.cl
         libclc/generic/lib/relational/isequal.cl
         libclc/generic/lib/relational/isfinite.cl
         libclc/generic/lib/relational/isgreater.cl
         libclc/generic/lib/relational/isgreaterequal.cl
         libclc/generic/lib/relational/isinf.cl
         libclc/generic/lib/relational/isless.cl
         libclc/generic/lib/relational/islessequal.cl
         libclc/generic/lib/relational/islessgreater.cl
         libclc/generic/lib/relational/isnan.cl
         libclc/generic/lib/relational/isnormal.cl
         libclc/generic/lib/relational/isnotequal.cl
         libclc/generic/lib/relational/isordered.cl
         libclc/generic/lib/relational/isunordered.cl
         libclc/generic/lib/relational/signbit.cl
         libclc/generic/lib/shared/clamp.cl
         libclc/generic/lib/shared/max.cl
         libclc/generic/lib/shared/min.cl
         libclc/generic/lib/shared/vload.cl
         libclc/generic/lib/shared/vstore.cl
         libclc/generic/lib/image/get_image_dim.cl
)

# build the prepare-builtins utility from libclc to set linkonce_odr
add_custom_command(
   OUTPUT prepare-builtins.o  
   COMMAND ${llvm}/bin/clang++ -pthread -I${CMAKE_SOURCE_DIR}/libclc/libclc_overrides/include -I${llvm}/include -fPIC -fvisibility-inlines-hidden -Wall -W -Wno-unused-parameter -Wwrite-strings -Wcast-qual -Wno-missing-field-initializers -pedantic -Wno-long-long -Wno-uninitialized -Wdelete-non-virtual-dtor -Wno-comment -std=c++11 -ffunction-sections -fdata-sections   -fno-exceptions -fno-rtti -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -fno-exceptions -fno-rtti -c -o prepare-builtins.o ${CMAKE_SOURCE_DIR}/libclc/utils/prepare-builtins.cpp
   DEPENDS "${CMAKE_SOURCE_DIR}/libclc/utils/prepare-builtins.cpp" 
)

add_custom_command(
   OUTPUT prepare-builtins  
   COMMAND ${llvm}/bin/clang++ -o prepare-builtins prepare-builtins.o ${llvm}/lib/libLLVMBitWriter.a ${llvm}/lib/libLLVMBitReader.a ${llvm}/lib/libLLVMCore.a ${llvm}/lib/libLLVMSupport.a -pthread -L${llvm}/lib -Wl,-rpath ${llvm}/lib -lncurses -ldl
   DEPENDS prepare-builtins.o
)

# build conversion routines
add_custom_command(
   OUTPUT convert.cl
   COMMAND python < ${CMAKE_SOURCE_DIR}/libclc/generic/lib/gen_convert.py > convert.cl
   DEPENDS ${CMAKE_SOURCE_DIR}/libclc/generic/lib/gen_convert.py 
)

# Loop over each of the listed mcpus
foreach(mcpu ${amdgcn_mcpus})

         # compile libclc files, a counter is used to create unique component names
         set(cn_count 0)
         foreach(cl_filename ${amdgcn_clfiles})
            set(full_filename ${CMAKE_SOURCE_DIR}/${cl_filename})
            get_filename_component(cn_noindex ${full_filename} NAME_WE) 
            get_filename_component(src_dirname ${full_filename} DIRECTORY) 
            math(EXPR cn_count "${cn_count} + 1")
            set(cn ${cn_noindex}_${cn_count})
            set(bc_files_${mcpu} ${bc_files_${mcpu}} ${cn}-${mcpu}.bc)
            set(component ${cn}-${mcpu})
            add_custom_command(
               OUTPUT ${cn}-${mcpu}.bc 
               COMMAND ${cl_cc_cmd} -mcpu=${mcpu} -I${src_dirname} ${CMAKE_SOURCE_DIR}/${cl_filename} -o ${cn}-${mcpu}.bc
               DEPENDS "${CMAKE_SOURCE_DIR}/${cl_filename}" 
            )
            add_custom_target(${component} DEPENDS ${cn}-${mcpu}.bc)
            set(components_list_${mcpu} ${components_list_${mcpu}} ${component})
         endforeach()

         foreach(ll_filename ${amdgcn_llfiles})
            set(full_filename ${CMAKE_SOURCE_DIR}/${ll_filename})
            get_filename_component(cn_noindex ${full_filename} NAME_WE) 
            math(EXPR cn_count "${cn_count} + 1")
            set(cn ${cn_noindex}_${cn_count})
            set(bc_files_${mcpu} ${bc_files_${mcpu}} ${cn}-${mcpu}.bc)
            set(component ${cn}-${mcpu})
            add_custom_command(
               OUTPUT ${cn}-${mcpu}.bc 
               COMMAND ${llvm}/bin/llvm-as ${CMAKE_SOURCE_DIR}/${ll_filename} -o ${cn}-${mcpu}.bc
               DEPENDS "${CMAKE_SOURCE_DIR}/${ll_filename}" 
            )
            add_custom_target(${component} DEPENDS ${cn}-${mcpu}.bc)
            set(components_list_${mcpu} ${components_list_${mcpu}} ${component})
         endforeach()

         add_custom_command(
            OUTPUT convert.${mcpu}.bc 
            COMMAND ${cl_cc_cmd} -mcpu=${mcpu} -o convert.${mcpu}.bc convert.cl 
            DEPENDS convert.cl
         )
         add_custom_target(convert-${mcpu} DEPENDS convert.${mcpu}.bc)
         set(components_list_${mcpu} ${components_list_${mcpu}} convert-${mcpu})
         set(bc_files_${mcpu} ${bc_files_${mcpu}} convert.${mcpu}.bc)

         # disable subnormal
         add_custom_command(
            OUTPUT subnormal_disable.${mcpu}.bc 
            COMMAND ${llvm}/bin/llvm-as ${CMAKE_SOURCE_DIR}/libclc/generic/lib/subnormal_disable.ll -o subnormal_disable.${mcpu}.bc
            DEPENDS ${CMAKE_SOURCE_DIR}/libclc/generic/lib/subnormal_disable.ll 
         )
         add_custom_target(subnormal_disable-${mcpu} DEPENDS subnormal_disable.${mcpu}.bc)
         set(components_list_${mcpu} ${components_list_${mcpu}} subnormal_disable-${mcpu})
         #set(bc_files_${mcpu} ${bc_files_${mcpu}} subnormal_disable.${mcpu}.bc)

         add_custom_command(
            OUTPUT linkout-${mcpu}.bc 
            COMMAND ${llvm}/bin/llvm-link --suppress-warnings  ${bc_files_${mcpu}} -o linkout-${mcpu}.bc 
            DEPENDS ${components_list_${mcpu}}
         )

         add_custom_command(
            OUTPUT optout-${mcpu}.bc 
            COMMAND ${llvm}/bin/opt -O${optimization_level} linkout-${mcpu}.bc  -o optout-${mcpu}.bc 
            DEPENDS linkout-${mcpu}.bc 
         )

         add_custom_command(
            OUTPUT libamdgcn.${mcpu}.bc  
            COMMAND ${CMAKE_CURRENT_BINARY_DIR}/prepare-builtins optout-${mcpu}.bc -o libamdgcn.${mcpu}.bc 
            DEPENDS optout-${mcpu}.bc prepare-builtins
         )

         add_custom_target(libamdgcn-${mcpu} DEPENDS libamdgcn.${mcpu}.bc)
         add_dependencies(LIBAMDGCN libamdgcn-${mcpu})

         install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libamdgcn.${mcpu}.bc DESTINATION "lib")

endforeach()

install(DIRECTORY ${CMAKE_SOURCE_DIR}/libclc/generic/include DESTINATION .)
# Override some includes from libclc
install(DIRECTORY ${CMAKE_SOURCE_DIR}/libclc_overrides/include DESTINATION .)


