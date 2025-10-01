; ModuleID = 'expr'
source_filename = "expr"

@0 = private unnamed_addr constant [18 x i8] c"expr value: [%d]\0A\00", align 1
@1 = private unnamed_addr constant [18 x i8] c"expr value: [%d]\0A\00", align 1
@2 = private unnamed_addr constant [18 x i8] c"expr value: [%d]\0A\00", align 1

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @0, i32 4)
  %1 = call i32 (ptr, ...) @printf(ptr @1, i32 10)
  %2 = call i32 (ptr, ...) @printf(ptr @2, i32 194)
  ret i32 0
}
