
; Function Attrs: alwaysinline
define i32 @get_local_size(i32) #0 {
  %dispatch_ptr = call noalias nonnull dereferenceable(64) i8 addrspace(2)* @llvm.amdgcn.dispatch.ptr()
  %dispatch_ptr_i32 = bitcast i8 addrspace(2)* %dispatch_ptr to i32 addrspace(2)*
  %size_xy_ptr = getelementptr inbounds i32, i32 addrspace(2)* %dispatch_ptr_i32, i64 1
  %size_xy = load i32, i32 addrspace(2)* %size_xy_ptr, align 4, !invariant.load !0
  switch i32 %0, label %8 [
    i32 0, label %2
    i32 1, label %4
    i32 2, label %6
  ]
; <label>:2                                       ; preds = %1
  %3 = and i32 %size_xy, 65535 ; 0xffff
  ret i32 %3

; <label>:4                                       ; preds = %1
  %5 = lshr i32 %size_xy, 16
  ret i32 %5

; <label>:6                                       ; preds = %1
  %size_z_ptr = getelementptr inbounds i32, i32 addrspace(2)* %dispatch_ptr_i32, i64 2
  %7 = load i32, i32 addrspace(2)* %size_z_ptr, align 4, !invariant.load !0, !range !1
  ret i32 %7

; <label>:8                                       ; preds = %1
  unreachable
}

declare i8 addrspace(2)* @llvm.amdgcn.dispatch.ptr() #1

!0 = !{}
!1 = !{i32 0, i32 257}

attributes #0 = { alwaysinline }
attributes #1 = { nounwind readnone }
attributes #2 = { nounwind convergent }
attributes #3 = { alwaysinline nounwind convergent }
