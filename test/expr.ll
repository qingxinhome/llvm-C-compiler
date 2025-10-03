; ModuleID = 'expr'
source_filename = "expr"

@0 = private unnamed_addr constant [18 x i8] c"expr value: [%d]\0A\00", align 1

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %a = alloca i32, align 4
  store i32 3, ptr %a, align 4
  %b = alloca i32, align 4
  store i32 4, ptr %b, align 4
  %a1 = load i32, ptr %a, align 4
  %b2 = load i32, ptr %b, align 4
  %0 = mul nsw i32 %b2, 4
  %1 = add nsw i32 %a1, %0
  %2 = add nsw i32 %1, 5
  %3 = call i32 (ptr, ...) @printf(ptr @0, i32 %2)
  ret i32 0
}
