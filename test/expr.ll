; ModuleID = 'expr'
source_filename = "expr"

@0 = private unnamed_addr constant [18 x i8] c"expr value: [%d]\0A\00", align 1

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %aa = alloca i32, align 4
  store i32 3, ptr %aa, align 4
  %aa1 = load i32, ptr %aa, align 4
  %b = alloca i32, align 4
  store i32 4, ptr %b, align 4
  %b2 = load i32, ptr %b, align 4
  store i32 15, ptr %b, align 4
  %b3 = load i32, ptr %b, align 4
  store i32 5, ptr %b, align 4
  %b4 = load i32, ptr %b, align 4
  store i32 %b4, ptr %aa, align 4
  %aa5 = load i32, ptr %aa, align 4
  %aa6 = load i32, ptr %aa, align 4
  %b7 = load i32, ptr %b, align 4
  %0 = mul nsw i32 %b7, 9
  %1 = add nsw i32 %aa6, %0
  %2 = call i32 (ptr, ...) @printf(ptr @0, i32 %1)
  ret i32 0
}
