int b = 20int main(){[5]int a = 0;a[2] = 3 * 4;return a[2];}; ModuleID = 'demo/eval1.c'
source_filename = "demo/eval1.c"

@0 = global i32 20, align 4

define i32 @main() {
entry:
  %a = alloca [5 x i32], align 4
  store i32 0, ptr %a, align 4
  %a1 = load [5 x i32], ptr %a, align 4
  %0 = getelementptr inbounds i32, ptr %a, i32 2
  %1 = load i32, ptr %0, align 4
  store i32 12, ptr %0, align 4
  %a2 = load [5 x i32], ptr %a, align 4
  %2 = getelementptr inbounds i32, ptr %a, i32 2
  %3 = load i32, ptr %2, align 4
  ret i32 %3
}
