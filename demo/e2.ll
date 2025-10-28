; ModuleID = 'expr'
source_filename = "expr"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %aa = alloca i32, align 4
  %b = alloca i32, align 4
  store i32 4, ptr %b, align 4
  %aa1 = load i32, ptr %aa, align 4
  store i32 1, ptr %aa, align 4
  %aa2 = load i32, ptr %aa, align 4
  %b3 = load i32, ptr %b, align 4
  store i32 -2, ptr %b, align 4
  store i32 -2, ptr %aa, align 4
  br label %cond

cond:                                             ; preds = %entry
  %b4 = load i32, ptr %b, align 4
  %0 = icmp ne i32 %b4, 0
  %1 = sext i1 %0 to i32
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %then, label %else

then:                                             ; preds = %cond
  br label %cond5

else:                                             ; preds = %cond
  %b12 = load i32, ptr %b, align 4
  store i32 19, ptr %b, align 4
  br label %last

last:                                             ; preds = %else, %last8
  br label %for.init

cond5:                                            ; preds = %then
  br i1 false, label %then6, label %else7

then6:                                            ; preds = %cond5
  %b9 = load i32, ptr %b, align 4
  store i32 9, ptr %b, align 4
  %aa10 = load i32, ptr %aa, align 4
  store i32 19, ptr %aa, align 4
  br label %last8

else7:                                            ; preds = %cond5
  %aa11 = load i32, ptr %aa, align 4
  store i32 30, ptr %aa, align 4
  br label %last8

last8:                                            ; preds = %else7, %then6
  br label %last

for.init:                                         ; preds = %last
  %i = alloca i32, align 4
  store i32 0, ptr %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %for.init
  %i13 = load i32, ptr %i, align 4
  %3 = icmp slt i32 %i13, 100
  %4 = sext i1 %3 to i32
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %for.body, label %for.last

for.inc:                                          ; preds = %for.last25
  %i36 = load i32, ptr %i, align 4
  %i37 = load i32, ptr %i, align 4
  %6 = add nsw i32 %i37, 1
  store i32 %6, ptr %i, align 4
  br label %for.cond

for.body:                                         ; preds = %for.cond
  %aa14 = load i32, ptr %aa, align 4
  %aa15 = load i32, ptr %aa, align 4
  %i16 = load i32, ptr %i, align 4
  %7 = add nsw i32 %aa15, %i16
  store i32 %7, ptr %aa, align 4
  br label %cond17

for.last:                                         ; preds = %then18, %for.cond
  %aa38 = load i32, ptr %aa, align 4
  %b39 = load i32, ptr %b, align 4
  %8 = mul nsw i32 %b39, 9
  %9 = add nsw i32 %aa38, %8
  ret i32 %9

cond17:                                           ; preds = %for.body
  %i20 = load i32, ptr %i, align 4
  %10 = icmp sge i32 %i20, 55
  %11 = sext i1 %10 to i32
  %12 = icmp ne i32 %11, 0
  br i1 %12, label %then18, label %last19

then18:                                           ; preds = %cond17
  br label %for.last

last19:                                           ; preds = %for.break.death, %cond17
  br label %for.init21

for.break.death:                                  ; No predecessors!
  br label %last19

for.init21:                                       ; preds = %last19
  %j = alloca i32, align 4
  store i32 1, ptr %j, align 4
  br label %for.cond22

for.cond22:                                       ; preds = %for.inc23, %for.init21
  %j26 = load i32, ptr %j, align 4
  %13 = icmp slt i32 %j26, 10
  %14 = sext i1 %13 to i32
  %15 = icmp ne i32 %14, 0
  br i1 %15, label %for.body24, label %for.last25

for.inc23:                                        ; preds = %last29, %then28
  %j34 = load i32, ptr %j, align 4
  %j35 = load i32, ptr %j, align 4
  %16 = add nsw i32 %j35, 2
  store i32 %16, ptr %j, align 4
  br label %for.cond22

for.body24:                                       ; preds = %for.cond22
  br label %cond27

for.last25:                                       ; preds = %for.cond22
  br label %for.inc

cond27:                                           ; preds = %for.body24
  %i30 = load i32, ptr %i, align 4
  %17 = icmp eq i32 %i30, 10
  %18 = sext i1 %17 to i32
  %19 = icmp ne i32 %18, 0
  br i1 %19, label %then28, label %last29

then28:                                           ; preds = %cond27
  br label %for.inc23

last29:                                           ; preds = %for.continue.death, %cond27
  %b31 = load i32, ptr %b, align 4
  %b32 = load i32, ptr %b, align 4
  %j33 = load i32, ptr %j, align 4
  %20 = mul nsw i32 %b32, %j33
  store i32 %20, ptr %b, align 4
  br label %for.inc23

for.continue.death:                               ; No predecessors!
  br label %last29
}
