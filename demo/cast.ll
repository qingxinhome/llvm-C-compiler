int printf(char * fmg, ...);int main(){int i = 17;char c = 99;int sum;sum = i + c;printf(Value of sum : %d\n,sum);return 0;}; ModuleID = 'demo/cast.c'
source_filename = "demo/cast.c"

@"Value of sum : %d\0A" = private constant [19 x i8] c"Value of sum : %d\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %sum = alloca i32, align 4
  %c = alloca i8, align 1
  %i = alloca i32, align 4
  store i32 17, ptr %i, align 4
  store i8 99, ptr %c, align 1
  %sum1 = load i32, ptr %sum, align 4
  %i2 = load i32, ptr %i, align 4
  %c3 = load i8, ptr %c, align 1
  %0 = sext i8 %c3 to i32
  %1 = add nsw i32 %i2, %0
  store i32 %1, ptr %sum, align 4
  %sum4 = load i32, ptr %sum, align 4
  %2 = call i32 (ptr, ...) @printf(ptr @"Value of sum : %d\0A", i32 %sum4)
  ret i32 0
}
Value of sum : 116
