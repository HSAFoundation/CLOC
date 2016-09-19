
; Function Attrs: alwaysinline nounwind 
define i32 @get_num_groups_ll(i32) #1 {
  %2 = tail call i8 addrspace(2)* @llvm.amdgcn.dispatch.ptr() #1
  switch i32 %0, label %24 [
    i32 0, label %3
    i32 1, label %10
    i32 2, label %17
  ]

; <label>:3:                                      ; preds = %1
  %4 = getelementptr inbounds i8, i8 addrspace(2)* %2, i64 12
  %5 = bitcast i8 addrspace(2)* %4 to i32 addrspace(2)*
  %6 = load i32, i32 addrspace(2)* %5, align 4, !tbaa !16
  %7 = getelementptr inbounds i8, i8 addrspace(2)* %2, i64 4
  %8 = bitcast i8 addrspace(2)* %7 to i16 addrspace(2)*
  %9 = load i16, i16 addrspace(2)* %8, align 4, !tbaa !9
  br label %24

; <label>:10:                                     ; preds = %1
  %11 = getelementptr inbounds i8, i8 addrspace(2)* %2, i64 16
  %12 = bitcast i8 addrspace(2)* %11 to i32 addrspace(2)*
  %13 = load i32, i32 addrspace(2)* %12, align 8, !tbaa !17
  %14 = getelementptr inbounds i8, i8 addrspace(2)* %2, i64 6
  %15 = bitcast i8 addrspace(2)* %14 to i16 addrspace(2)*
  %16 = load i16, i16 addrspace(2)* %15, align 2, !tbaa !14
  br label %24

; <label>:17:                                     ; preds = %1
  %18 = getelementptr inbounds i8, i8 addrspace(2)* %2, i64 20
  %19 = bitcast i8 addrspace(2)* %18 to i32 addrspace(2)*
  %20 = load i32, i32 addrspace(2)* %19, align 4, !tbaa !18
  %21 = getelementptr inbounds i8, i8 addrspace(2)* %2, i64 8
  %22 = bitcast i8 addrspace(2)* %21 to i16 addrspace(2)*
  %23 = load i16, i16 addrspace(2)* %22, align 8, !tbaa !15
  br label %24

; <label>:24:                                     ; preds = %17, %10, %3, %1
  %25 = phi i16 [ %23, %17 ], [ %16, %10 ], [ %9, %3 ], [ 1, %1 ]
  %26 = phi i32 [ %20, %17 ], [ %13, %10 ], [ %6, %3 ], [ 1, %1 ]
  %27 = zext i16 %25 to i32
  %28 = udiv i32 %26, %27
  %29 = mul i32 %28, %27
  %30 = icmp ugt i32 %26, %29
  %31 = zext i1 %30 to i32
  %32 = add i32 %31, %28
  ret i32 %32
}

attributes #1 = { alwaysinline nounwind }
attributes #2 = { alwaysinline nounwind readnone "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-features"="+fp64-denormals,-fp32-denormals" "unsafe-fp-math"="false" "use-soft-float"="false" }

declare i8 addrspace(2)* @llvm.amdgcn.dispatch.ptr() #1

!0 = !{i32 2, i32 0}
!1 = !{!"clang version 4.0 "}
!2 = !{!3, !3, i64 0}
!3 = !{!"omnipotent char", !4, i64 0}
!4 = !{!"Simple C/C++ TBAA"}
!5 = !{!6, !6, i64 0}
!6 = !{!"int", !3, i64 0}
!7 = !{!8, !8, i64 0}
!8 = !{!"long", !3, i64 0}
!9 = !{!10, !11, i64 4}
!10 = !{!"hsa_kernel_dispatch_packet_s", !11, i64 0, !11, i64 2, !11, i64 4, !11, i64 6, !11, i64 8, !11, i64 10, !6, i64 12, !6, i64 16, !6, i64 20, !6, i64 24, !6, i64 28, !8, i64 32, !12, i64 40, !6, i64 48, !8, i64 56, !13, i64 64}
!11 = !{!"short", !3, i64 0}
!12 = !{!"any pointer", !3, i64 0}
!13 = !{!"hsa_signal_s", !8, i64 0}
!14 = !{!10, !11, i64 6}
!15 = !{!10, !11, i64 8}
!16 = !{!10, !6, i64 12}
!17 = !{!10, !6, i64 16}
!18 = !{!10, !6, i64 20}
!19 = !{!10, !11, i64 2}
