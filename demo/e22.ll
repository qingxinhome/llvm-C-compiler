int printf(char * fmg, ...);int main(){char *str = 学嵌入式，到宅学部落;printf(str: %s\n,str);return 0;}; ModuleID = 'demo/e22.c'
source_filename = "demo/e22.c"

@"\E5\AD\A6\E5\B5\8C\E5\85\A5\E5\BC\8F\EF\BC\8C\E5\88\B0\E5\AE\85\E5\AD\A6\E9\83\A8\E8\90\BD" = private constant [31 x i8] c"\E5\AD\A6\E5\B5\8C\E5\85\A5\E5\BC\8F\EF\BC\8C\E5\88\B0\E5\AE\85\E5\AD\A6\E9\83\A8\E8\90\BD\00"
@"str: %s\0A" = private constant [9 x i8] c"str: %s\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %str = alloca ptr, align 8
  store ptr @"\E5\AD\A6\E5\B5\8C\E5\85\A5\E5\BC\8F\EF\BC\8C\E5\88\B0\E5\AE\85\E5\AD\A6\E9\83\A8\E8\90\BD", ptr %str, align 8
  %str1 = load ptr, ptr %str, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @"str: %s\0A", ptr %str1)
  ret i32 0
}
str: 学嵌入式，到宅学部落
