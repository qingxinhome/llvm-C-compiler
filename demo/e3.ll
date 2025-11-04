; ModuleID = 'demo/e3.c'
source_filename = "demo/e3.c"

@0 = global i32 10, align 4

define i32 @main() {
entry:
  br label %cond

cond:                                             ; preds = %entry
  %a = load i32, ptr @0, align 4
  %0 = icmp ne i32 %a, 10
  %1 = sext i1 %0 to i32
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %then, label %else

then:                                             ; preds = %cond
  %a1 = load i32, ptr @0, align 4
  store i32 2, ptr @0, align 4
  br label %last

else:                                             ; preds = %cond
  %a2 = load i32, ptr @0, align 4
  store i32 20, ptr @0, align 4
  br label %last

last:                                             ; preds = %else, %then
  %a3 = load i32, ptr @0, align 4
  ret i32 %a3
}
