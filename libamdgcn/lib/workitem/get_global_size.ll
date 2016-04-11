declare i8 addrspace(2)* @llvm.amdgcn.dispatch.ptr() #1

; Function Attrs: alwaysinline nounwind
define i32 @get_global_size(i32 %dim) #5 {
entry:
  %call = tail call noalias nonnull dereferenceable(64) i8 addrspace(2)* @llvm.amdgcn.dispatch.ptr()
  %dispatch_ptr_i32 = bitcast i8 addrspace(2)* %call to i32 addrspace(2)*
  switch i32 %dim, label %cleanup [
    i32 0, label %if.then
    i32 1, label %if.then2
    i32 2, label %if.then5
  ]

if.then:                                          ; preds = %entry
  %grid_size_x_ptr = getelementptr inbounds i32, i32 addrspace(2)* %dispatch_ptr_i32, i64 3
  %grid_size_x = load i32, i32 addrspace(2)* %grid_size_x_ptr, align 4, !invariant.load !0
  ret i32 %grid_size_x

if.then2:                                         ; preds = %entry
  %grid_size_y_ptr = getelementptr inbounds i32, i32 addrspace(2)* %dispatch_ptr_i32, i64 4
  %grid_size_y = load i32, i32 addrspace(2)* %grid_size_y_ptr, align 4, !invariant.load !0
  ret i32 %grid_size_y

if.then5:                                         ; preds = %entry
  %grid_size_z_ptr = getelementptr inbounds i32, i32 addrspace(2)* %dispatch_ptr_i32, i64 5
  %grid_size_z = load i32, i32 addrspace(2)* %grid_size_z_ptr, align 4, !invariant.load !0
  ret i32 %grid_size_z

cleanup:                                          ; preds = %if.then5, %if.then2, %if.then, %entry
  unreachable
  ret i32 0
}

!0 = !{}
!1 = !{i32 0, i32 257}

attributes #0 = { alwaysinline }
attributes #1 = { nounwind readnone }
attributes #2 = { nounwind convergent }
attributes #3 = { alwaysinline nounwind convergent }
