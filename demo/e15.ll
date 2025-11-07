; ModuleID = 'demo/e15.c'
source_filename = "demo/e15.c"

@0 = global ptr @"hello,world\0A", align 8
@"hello,world\0A" = private constant [13 x i8] c"hello,world\0A\00"
@"%s\0A" = private constant [4 x i8] c"%s\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %b = alloca [3 x i8], align 1
  %a = alloca [3 x i8], align 1
  %0 = getelementptr inbounds [3 x i8], ptr %a, i32 0, i32 0
  store i32 104, ptr %0, align 4
  %1 = getelementptr inbounds [3 x i8], ptr %a, i32 0, i32 1
  store i32 104, ptr %1, align 4
  %2 = getelementptr inbounds [3 x i8], ptr %a, i32 0, i32 2
  store i32 104, ptr %2, align 4
  %3 = getelementptr inbounds [3 x i8], ptr %b, i32 0, i32 0
  store i32 104, ptr %3, align 4
  %4 = getelementptr inbounds [3 x i8], ptr %b, i32 0, i32 1
  store i32 104, ptr %4, align 4
  %5 = getelementptr inbounds [3 x i8], ptr %b, i32 0, i32 2
  store i32 104, ptr %5, align 4
  %hello = load ptr, ptr @0, align 8
  %6 = call i32 (ptr, ...) @printf(ptr @"%s\0A", ptr %hello)
  ret i32 0
}
