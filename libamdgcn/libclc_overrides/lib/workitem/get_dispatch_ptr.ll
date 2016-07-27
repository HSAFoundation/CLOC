declare i8 addrspace(2)* @llvm.amdgcn.dispatch.ptr() #0

; Function Attrs: alwaysinline nounwind readnone
define i8 addrspace(2)* @get_dispatch_ptr() #1 {
  %dispatch_ptr = call noalias nonnull dereferenceable(64) i8 addrspace(2)* @llvm.amdgcn.dispatch.ptr()
  ret i8 addrspace(2)* %dispatch_ptr
}

attributes #0 = { nounwind readnone }
attributes #1 = { alwaysinline nounwind readnone }
