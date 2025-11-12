int main(){int a = 0;int b = 3;a = 2;return a;}; ModuleID = 'demo/e19.c'
source_filename = "demo/e19.c"

define i32 @main() {
entry:
  %b = alloca i32, align 4
  %a = alloca i32, align 4
  store i32 0, ptr %a, align 4
  store i32 3, ptr %b, align 4
  %a1 = load i32, ptr %a, align 4
  store i32 2, ptr %a, align 4
  %a2 = load i32, ptr %a, align 4
  ret i32 %a2
}
