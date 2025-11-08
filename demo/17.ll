; ModuleID = 'demo/e17.c'
source_filename = "demo/e17.c"

define i32 @main() {
entry:
  %a = alloca i32, align 4
  store i32 10, ptr %a, align 4
  br label %do.while.body

do.while.body:                                    ; preds = %do.while.cond, %entry
  %a1 = load i32, ptr %a, align 4
  %0 = icmp eq i32 %a1, 43
  %1 = sext i1 %0 to i32
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %then, label %last

then:                                             ; preds = %do.while.body
  br label %do.while.then

last:                                             ; preds = %do.while.body
  %a2 = load i32, ptr %a, align 4
  %3 = add i32 %a2, 3
  store i32 %3, ptr %a, align 4
  br label %do.while.cond

do.while.cond:                                    ; preds = %last
  %a3 = load i32, ptr %a, align 4
  %4 = icmp slt i32 %a3, 100
  %5 = sext i1 %4 to i32
  %6 = icmp eq i32 %5, 0
  br i1 %6, label %do.while.body, label %do.while.then

do.while.then:                                    ; preds = %do.while.cond, %then
  %a4 = load i32, ptr %a, align 4
  ret i32 %a4
}
