int printf(char * fmg, ...);int main(){int sum = 17;int count = 5;mean;mean = ()sum / count;printf(Value of mean : %f\n,mean);return 0;}; ModuleID = 'demo/cast2.c'
source_filename = "demo/cast2.c"

@"Value of mean : %f\0A" = private constant [20 x i8] c"Value of mean : %f\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %mean = alloca double, align 8
  %count = alloca i32, align 4
  %sum = alloca i32, align 4
  store i32 17, ptr %sum, align 4
  store i32 5, ptr %count, align 4
  %mean1 = load double, ptr %mean, align 8
  %sum2 = load i32, ptr %sum, align 4
  %0 = sitofp i32 %sum2 to double
  %count3 = load i32, ptr %count, align 4
  %1 = sitofp i32 %count3 to double
  %2 = fdiv double %0, %1
  store double %2, ptr %mean, align 8
  %mean4 = load double, ptr %mean, align 8
  %3 = call i32 (ptr, ...) @printf(ptr @"Value of mean : %f\0A", double %mean4)
  ret i32 0
}
Value of mean : 3.400000
