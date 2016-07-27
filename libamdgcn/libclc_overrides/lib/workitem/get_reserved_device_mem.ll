declare i8 addrspace(2)* @llvm.amdgcn.dispatch.ptr() #0

; Function Attrs: alwaysinline nounwind convergent
define i32 addrspace(1)* @get_reserved_device_mem(i32 %dim) #1 {
entry:
  %call = tail call noalias nonnull dereferenceable(64) i8 addrspace(2)* @llvm.amdgcn.dispatch.ptr()
  ;%dispatch_ptr_i32 = bitcast i8 addrspace(2)* %call to i32 addrspace(1)*
  %dispatch_ptr_i32 = addrspacecast i8 addrspace(2)* %call to i32 addrspace(1)*
  %reserved_device_mem_ptr = getelementptr inbounds i32, i32 addrspace(1)* %dispatch_ptr_i32, i32 12 
  %reserved_device_mem_val = getelementptr inbounds i32, i32 addrspace(1)* %reserved_device_mem_ptr, i32 %dim
  ret i32 addrspace(1)*  %reserved_device_mem_val
}

define i32 addrspace(1)* @get_reserved_device_mem_ptr() #1 {
entry:
  %call = tail call noalias nonnull dereferenceable(64) i8 addrspace(2)* @llvm.amdgcn.dispatch.ptr()
  ;%dispatch_ptr_i32 = bitcast i8 addrspace(2)* %call to i32 addrspace(1)*
  %dispatch_ptr_i32 = addrspacecast i8 addrspace(2)* %call to i32 addrspace(1)*
  %reserved_device_mem_ptr = getelementptr inbounds i32, i32 addrspace(1)* %dispatch_ptr_i32, i32 12 
  ret i32 addrspace(1)*  %reserved_device_mem_ptr
}

!0 = !{}

attributes #0 = { nounwind readnone }
attributes #1 = { alwaysinline nounwind convergent }
