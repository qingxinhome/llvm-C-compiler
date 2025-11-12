int main(){struct _1anony_struct_1_{int a;int b;} p = 1,2;return p.a + p.b;}; ModuleID = 'demo/typedef.c'
source_filename = "demo/typedef.c"

%_1anony_struct_1_ = type { i32, i32 }

define i32 @main() {
entry:
  %p = alloca %_1anony_struct_1_, align 4
  %0 = getelementptr inbounds %_1anony_struct_1_, ptr %p, i32 0, i32 0
  store i32 1, ptr %0, align 4
  %1 = getelementptr inbounds %_1anony_struct_1_, ptr %p, i32 0, i32 1
  store i32 2, ptr %1, align 4
  %p1 = load %_1anony_struct_1_, ptr %p, align 4
  %2 = getelementptr inbounds %_1anony_struct_1_, ptr %p, i32 0, i32 0
  %3 = load i32, ptr %2, align 4
  %p2 = load %_1anony_struct_1_, ptr %p, align 4
  %4 = getelementptr inbounds %_1anony_struct_1_, ptr %p, i32 0, i32 1
  %5 = load i32, ptr %4, align 4
  %6 = add nsw i32 %3, %5
  ret i32 %6
}
