; ModuleID = 'demo/e16.c'
source_filename = "demo/e16.c"

@"\E5\BE\88\E6\A3\92\EF\BC\81\0A" = private constant [11 x i8] c"\E5\BE\88\E6\A3\92\EF\BC\81\0A\00"
@"\E5\81\9A\E5\BE\97\E5\A5\BD\0A" = private constant [11 x i8] c"\E5\81\9A\E5\BE\97\E5\A5\BD\0A\00"
@"\E6\82\A8\E9\80\9A\E8\BF\87\E4\BA\86\0A" = private constant [14 x i8] c"\E6\82\A8\E9\80\9A\E8\BF\87\E4\BA\86\0A\00"
@"\E6\9C\80\E5\A5\BD\E5\86\8D\E8\AF\95\E4\B8\80\E4\B8\8B\0A" = private constant [20 x i8] c"\E6\9C\80\E5\A5\BD\E5\86\8D\E8\AF\95\E4\B8\80\E4\B8\8B\0A\00"
@"\E6\97\A0\E6\95\88\E7\9A\84\E6\88\90\E7\BB\A9\0A" = private constant [17 x i8] c"\E6\97\A0\E6\95\88\E7\9A\84\E6\88\90\E7\BB\A9\0A\00"
@"\E6\82\A8\E7\9A\84\E6\88\90\E7\BB\A9\E6\98\AF %c\0A" = private constant [20 x i8] c"\E6\82\A8\E7\9A\84\E6\88\90\E7\BB\A9\E6\98\AF %c\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %ret = alloca i32, align 4
  %grade = alloca i8, align 1
  store i8 69, ptr %grade, align 1
  %grade1 = load i8, ptr %grade, align 1
  %0 = sext i8 %grade1 to i32
  store i32 %0, ptr %ret, align 4
  %grade2 = load i8, ptr %grade, align 1
  switch i8 %grade2, label %switch.default [
    i8 65, label %case
    i8 66, label %case3
    i8 67, label %case4
    i8 68, label %case6
    i8 70, label %case7
  ]

case:                                             ; preds = %entry
  %1 = call i32 (ptr, ...) @printf(ptr @"\E5\BE\88\E6\A3\92\EF\BC\81\0A")
  br label %switch.default8

case3:                                            ; preds = %entry
  br label %case4

case4:                                            ; preds = %entry, %case3
  %ret5 = load i32, ptr %ret, align 4
  %2 = add i32 %ret5, 1
  store i32 %2, ptr %ret, align 4
  %3 = call i32 (ptr, ...) @printf(ptr @"\E5\81\9A\E5\BE\97\E5\A5\BD\0A")
  br label %switch.default8

case6:                                            ; preds = %entry
  %4 = call i32 (ptr, ...) @printf(ptr @"\E6\82\A8\E9\80\9A\E8\BF\87\E4\BA\86\0A")
  br label %switch.default8

case7:                                            ; preds = %entry
  %5 = call i32 (ptr, ...) @printf(ptr @"\E6\9C\80\E5\A5\BD\E5\86\8D\E8\AF\95\E4\B8\80\E4\B8\8B\0A")
  br label %switch.default8

switch.default:                                   ; preds = %entry
  %6 = call i32 (ptr, ...) @printf(ptr @"\E6\97\A0\E6\95\88\E7\9A\84\E6\88\90\E7\BB\A9\0A")
  br label %switch.default8

switch.default8:                                  ; preds = %switch.default, %case7, %case6, %case4, %case
  %grade9 = load i8, ptr %grade, align 1
  %7 = call i32 (ptr, ...) @printf(ptr @"\E6\82\A8\E7\9A\84\E6\88\90\E7\BB\A9\E6\98\AF %c\0A", i8 %grade9)
  %ret10 = load i32, ptr %ret, align 4
  ret i32 %ret10
}
