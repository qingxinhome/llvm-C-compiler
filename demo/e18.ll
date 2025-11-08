; ModuleID = 'demo/e18.c'
source_filename = "demo/e18.c"

define i32 @main() {
entry:
  %a = alloca i32, align 4
  store i32 10, ptr %a, align 4
  br label %for.cond

for.cond:                                         ; preds = %last, %entry
  %a1 = load i32, ptr %a, align 4
  %0 = icmp slt i32 %a1, 100
  %1 = sext i1 %0 to i32
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %for.body, label %for.last

for.body:                                         ; preds = %for.cond
  %a2 = load i32, ptr %a, align 4
  %3 = icmp eq i32 %a2, 43
  %4 = sext i1 %3 to i32
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %then, label %last

then:                                             ; preds = %for.body
  br label %for.last

last:                                             ; preds = %for.body
  %a3 = load i32, ptr %a, align 4
  %6 = add i32 %a3, 3
  store i32 %6, ptr %a, align 4
  br label %for.cond

for.last:                                         ; preds = %then, %for.cond
  %a4 = load i32, ptr %a, align 4
  ret i32 %a4
}
