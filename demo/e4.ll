; ModuleID = 'demo/e4.c'
source_filename = "demo/e4.c"

define i32 @main() {
entry:
  %count = alloca i32, align 4
  %a = alloca i32, align 4
  store i32 0, ptr %a, align 4
  store i32 0, ptr %count, align 4
  br label %for.init

for.init:                                         ; preds = %entry
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %for.init
  %a1 = load i32, ptr %a, align 4
  %0 = icmp sle i32 %a1, 0
  %1 = sext i1 %0 to i32
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %for.body, label %for.last

for.body:                                         ; preds = %for.cond
  %a2 = load i32, ptr %a, align 4
  %a3 = load i32, ptr %a, align 4
  %3 = sub nsw i32 %a3, 1
  store i32 %3, ptr %a, align 4
  %count4 = load i32, ptr %count, align 4
  %count5 = load i32, ptr %count, align 4
  %4 = add nsw i32 %count5, 1
  store i32 %4, ptr %count, align 4
  br label %cond

cond:                                             ; preds = %for.body
  %a6 = load i32, ptr %a, align 4
  %5 = icmp slt i32 %a6, -20
  %6 = sext i1 %5 to i32
  %7 = icmp ne i32 %6, 0
  br i1 %7, label %then, label %last

then:                                             ; preds = %cond
  br label %for.last

for.break.death:                                  ; No predecessors!
  br label %last

last:                                             ; preds = %for.break.death, %cond
  br label %for.inc

for.inc:                                          ; preds = %last
  br label %for.cond

for.last:                                         ; preds = %then, %for.cond
  %count7 = load i32, ptr %count, align 4
  ret i32 %count7
}
