  %2 = icmp ne i32 %1, 0  %13 = icmp ne i32 %12, 0; ModuleID = 'demo/e1.c'
source_filename = "demo/e1.c"

@"\E9\80\9A\E8\BF\87\E4\BA\86\EF\BC\81\E5\9B\A0\E4\B8\BA b > 0\0A" = private constant [26 x i8] c"\E9\80\9A\E8\BF\87\E4\BA\86\EF\BC\81\E5\9B\A0\E4\B8\BA b > 0\0A\00"
@"\E6\B2\A1\E9\80\9A\E8\BF\87\0A" = private constant [11 x i8] c"\E6\B2\A1\E9\80\9A\E8\BF\87\0A\00"
@"\E6\88\91\E8\A2\AB\E6\89\A7\E8\A1\8C\E4\BA\86\EF\BC\81\0A" = private constant [20 x i8] c"\E6\88\91\E8\A2\AB\E6\89\A7\E8\A1\8C\E4\BA\86\EF\BC\81\0A\00"
@"a>0 \E6\97\B6\E8\BF\99\E9\87\8C\E4\B8\8D\E4\BC\9A\E6\89\93\E5\8D\B0\E7\AC\AC\E4\BA\8C\E5\8F\A5\0A" = private constant [36 x i8] c"a>0 \E6\97\B6\E8\BF\99\E9\87\8C\E4\B8\8D\E4\BC\9A\E6\89\93\E5\8D\B0\E7\AC\AC\E4\BA\8C\E5\8F\A5\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %b = alloca i32, align 4
  %a = alloca i32, align 4
  store i32 0, ptr %a, align 4
  store i32 1, ptr %b, align 4
  %a1 = load i32, ptr %a, align 4
  %0 = icmp sgt i32 %a1, 0
  %1 = sext i1 %0 to i32
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %nextBB, label %falseBB

nextBB:                                           ; preds = %entry
  %b2 = load i32, ptr %b, align 4
  %3 = icmp sgt i32 %b2, 0
  %4 = sext i1 %3 to i32
  %5 = icmp ne i32 %4, 0
  %6 = zext i1 %5 to i32
  br label %mergeBB

falseBB:                                          ; preds = %entry
  br label %mergeBB

mergeBB:                                          ; preds = %falseBB, %nextBB
  %7 = phi i32 [ %6, %nextBB ], [ 0, %falseBB ]
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %then, label %else

then:                                             ; preds = %mergeBB
  %9 = call i32 (ptr, ...) @printf(ptr @"\E9\80\9A\E8\BF\87\E4\BA\86\EF\BC\81\E5\9B\A0\E4\B8\BA b > 0\0A")
  br label %last

else:                                             ; preds = %mergeBB
  %10 = call i32 (ptr, ...) @printf(ptr @"\E6\B2\A1\E9\80\9A\E8\BF\87\0A")
  br label %last

last:                                             ; preds = %else, %then
  %a3 = load i32, ptr %a, align 4
  %11 = icmp sgt i32 %a3, 0
  %12 = sext i1 %11 to i32
  %13 = icmp ne i32 %12, 0
  br i1 %13, label %nextBB4, label %falseBB5

nextBB4:                                          ; preds = %last
  %14 = call i32 (ptr, ...) @printf(ptr @"\E6\88\91\E8\A2\AB\E6\89\A7\E8\A1\8C\E4\BA\86\EF\BC\81\0A")
  %15 = icmp ne i32 %14, 0
  %16 = zext i1 %15 to i32
  br label %mergeBB6

falseBB5:                                         ; preds = %last
  br label %mergeBB6

mergeBB6:                                         ; preds = %falseBB5, %nextBB4
  %17 = phi i32 [ %16, %nextBB4 ], [ 0, %falseBB5 ]
  %18 = icmp ne i32 %17, 0
  br i1 %18, label %then7, label %last8

then7:                                            ; preds = %mergeBB6
  %19 = call i32 (ptr, ...) @printf(ptr @"a>0 \E6\97\B6\E8\BF\99\E9\87\8C\E4\B8\8D\E4\BC\9A\E6\89\93\E5\8D\B0\E7\AC\AC\E4\BA\8C\E5\8F\A5\0A")
  br label %last8

last8:                                            ; preds = %then7, %mergeBB6
  ret i32 0
}