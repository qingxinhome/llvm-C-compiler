; ModuleID = 'expr'
source_filename = "expr"

@0 = private unnamed_addr constant [18 x i8] c"expr value: [%d]\0A\00", align 1

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %aa = alloca i32, align 4
  %b = alloca i32, align 4
  store i32 4, ptr %b, align 4
  %b1 = load i32, ptr %b, align 4
  store i32 1, ptr %aa, align 4
  %aa2 = load i32, ptr %aa, align 4
  store i32 -2, ptr %b, align 4
  %b3 = load i32, ptr %b, align 4
  store i32 %b3, ptr %aa, align 4
  %aa4 = load i32, ptr %aa, align 4
  br label %cond

cond:                                             ; preds = %entry
  %b5 = load i32, ptr %b, align 4
  %0 = icmp ne i32 %b5, 0
  %1 = sext i1 %0 to i32
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %then, label %else

then:                                             ; preds = %cond
  br label %cond6

else:                                             ; preds = %cond
  store i32 19, ptr %b, align 4
  %b12 = load i32, ptr %b, align 4
  br label %last

last:                                             ; preds = %else, %last9
  br label %for.init

cond6:                                            ; preds = %then
  br i1 false, label %then7, label %else8

then7:                                            ; preds = %cond6
  store i32 9, ptr %b, align 4
  %b10 = load i32, ptr %b, align 4
  br label %last9

else8:                                            ; preds = %cond6
  store i32 30, ptr %aa, align 4
  %aa11 = load i32, ptr %aa, align 4
  br label %last9

last9:                                            ; preds = %else8, %then7
  br label %last

for.init:                                         ; preds = %last
  %i = alloca i32, align 4
  store i32 0, ptr %i, align 4
  %i13 = load i32, ptr %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %for.init
  %i14 = load i32, ptr %i, align 4
  %3 = icmp slt i32 %i14, 100
  %4 = sext i1 %3 to i32
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %for.body, label %for.last

for.inc:                                          ; preds = %for.last26
  %i38 = load i32, ptr %i, align 4
  %6 = add nsw i32 %i38, 1
  store i32 %6, ptr %i, align 4
  %i39 = load i32, ptr %i, align 4
  br label %for.cond

for.body:                                         ; preds = %for.cond
  %aa15 = load i32, ptr %aa, align 4
  %i16 = load i32, ptr %i, align 4
  %7 = add nsw i32 %aa15, %i16
  store i32 %7, ptr %aa, align 4
  %aa17 = load i32, ptr %aa, align 4
  br label %cond18

for.last:                                         ; preds = %then19, %for.cond
  %aa40 = load i32, ptr %aa, align 4
  %b41 = load i32, ptr %b, align 4
  %8 = mul nsw i32 %b41, 9
  %9 = add nsw i32 %aa40, %8
  %10 = call i32 (ptr, ...) @printf(ptr @0, i32 %9)
  ret i32 0

cond18:                                           ; preds = %for.body
  %i21 = load i32, ptr %i, align 4
  %11 = icmp sge i32 %i21, 55
  %12 = sext i1 %11 to i32
  %13 = icmp ne i32 %12, 0
  br i1 %13, label %then19, label %last20

then19:                                           ; preds = %cond18
  br label %for.last

last20:                                           ; preds = %for.break.death, %cond18
  br label %for.init22

for.break.death:                                  ; No predecessors!
  br label %last20

for.init22:                                       ; preds = %last20
  %j = alloca i32, align 4
  store i32 1, ptr %j, align 4
  %j27 = load i32, ptr %j, align 4
  br label %for.cond23

for.cond23:                                       ; preds = %for.inc24, %for.init22
  %j28 = load i32, ptr %j, align 4
  %14 = icmp slt i32 %j28, 10
  %15 = sext i1 %14 to i32
  %16 = icmp ne i32 %15, 0
  br i1 %16, label %for.body25, label %for.last26

for.inc24:                                        ; preds = %last31, %then30
  %j36 = load i32, ptr %j, align 4
  %17 = add nsw i32 %j36, 2
  store i32 %17, ptr %j, align 4
  %j37 = load i32, ptr %j, align 4
  br label %for.cond23

for.body25:                                       ; preds = %for.cond23
  br label %cond29

for.last26:                                       ; preds = %for.cond23
  br label %for.inc

cond29:                                           ; preds = %for.body25
  %i32 = load i32, ptr %i, align 4
  %18 = icmp eq i32 %i32, 10
  %19 = sext i1 %18 to i32
  %20 = icmp ne i32 %19, 0
  br i1 %20, label %then30, label %last31

then30:                                           ; preds = %cond29
  br label %for.inc24

last31:                                           ; preds = %for.continue.death, %cond29
  %b33 = load i32, ptr %b, align 4
  %j34 = load i32, ptr %j, align 4
  %21 = mul nsw i32 %b33, %j34
  store i32 %21, ptr %b, align 4
  %b35 = load i32, ptr %b, align 4
  br label %for.inc24

for.continue.death:                               ; No predecessors!
  br label %last31
}
