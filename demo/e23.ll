int printf(char * fmg, ...);sum( a,int  b){return a + b;}int main(){(( a,int  b)*fp = sum;printf(%f\n,fp(1,2));return 0;}; ModuleID = 'demo/e23.c'
source_filename = "demo/e23.c"

@"%f\0A" = private constant [4 x i8] c"%f\0A\00"

declare i32 @printf(ptr, ...)

define float @sum(double %a, i32 %b) {
entry:
  %a1 = alloca double, align 8
  store double %a, ptr %a1, align 8
  %b2 = alloca i32, align 4
  store i32 %b, ptr %b2, align 4
  %a3 = load double, ptr %a1, align 8
  %b4 = load i32, ptr %b2, align 4
  %0 = sitofp i32 %b4 to double
  %1 = fadd double %a3, %0
  %2 = fptrunc double %1 to float
  ret float %2
}

define i32 @main() {
entry:
  %fp = alloca ptr, align 8
  store ptr @sum, ptr %fp, align 8
  %fp1 = load ptr, ptr %fp, align 8
  %0 = call float %fp1(double 1.000000e+00, i32 2)
  %1 = call i32 (ptr, ...) @printf(ptr @"%f\0A", float %0)
  ret i32 0
}
3.000000
