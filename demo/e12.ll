; ModuleID = 'demo/e12.c'
source_filename = "demo/e12.c"

define void @swap(ptr %p, ptr %q) {
entry:
  %t = alloca i32, align 4
  %p1 = alloca ptr, align 8
  store ptr %p, ptr %p1, align 8
  %q2 = alloca ptr, align 8
  store ptr %q, ptr %q2, align 8
  %p3 = load ptr, ptr %p1, align 8
  %0 = load i32, ptr %p3, align 4
  store i32 %0, ptr %t, align 4
  %p4 = load ptr, ptr %p1, align 8
  %1 = load i32, ptr %p4, align 4
  %q5 = load ptr, ptr %q2, align 8
  %2 = load i32, ptr %q5, align 4
  store i32 %2, ptr %p4, align 4
  %q6 = load ptr, ptr %q2, align 8
  %3 = load i32, ptr %q6, align 4
  %t7 = load i32, ptr %t, align 4
  store i32 %t7, ptr %q6, align 4
  ret void
}

define i32 @main() {
entry:
  %b = alloca i32, align 4
  %a = alloca i32, align 4
  store i32 3, ptr %a, align 4
  store i32 6, ptr %b, align 4
  %a1 = load i32, ptr %a, align 4
  %b2 = load i32, ptr %b, align 4
  call void @swap(ptr %a, ptr %b)
  %a3 = load i32, ptr %a, align 4
  ret i32 %a3
}
