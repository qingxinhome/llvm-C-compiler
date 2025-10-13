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
  br label %cond40

cond18:                                           ; preds = %for.body
  %i21 = load i32, ptr %i, align 4
  %8 = icmp sge i32 %i21, 55
  %9 = sext i1 %8 to i32
  %10 = icmp ne i32 %9, 0
  br i1 %10, label %then19, label %last20

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
  %11 = icmp slt i32 %j28, 10
  %12 = sext i1 %11 to i32
  %13 = icmp ne i32 %12, 0
  br i1 %13, label %for.body25, label %for.last26

for.inc24:                                        ; preds = %last31, %then30
  %j36 = load i32, ptr %j, align 4
  %14 = add nsw i32 %j36, 2
  store i32 %14, ptr %j, align 4
  %j37 = load i32, ptr %j, align 4
  br label %for.cond23

for.body25:                                       ; preds = %for.cond23
  br label %cond29

for.last26:                                       ; preds = %for.cond23
  br label %for.inc

cond29:                                           ; preds = %for.body25
  %i32 = load i32, ptr %i, align 4
  %15 = icmp eq i32 %i32, 10
  %16 = sext i1 %15 to i32
  %17 = icmp ne i32 %16, 0
  br i1 %17, label %then30, label %last31

then30:                                           ; preds = %cond29
  br label %for.inc24

last31:                                           ; preds = %for.continue.death, %cond29
  %b33 = load i32, ptr %b, align 4
  %j34 = load i32, ptr %j, align 4
  %18 = mul nsw i32 %b33, %j34
  store i32 %18, ptr %b, align 4
  %b35 = load i32, ptr %b, align 4
  br label %for.inc24

for.continue.death:                               ; No predecessors!
  br label %last31

cond40:                                           ; preds = %for.last
  %aa43 = load i32, ptr %aa, align 4
  %19 = icmp ne i32 %aa43, 0
  br i1 %19, label %trueBB, label %nextBB

then41:                                           ; preds = %mergeBB
  %aa48 = load i32, ptr %aa, align 4
  %20 = add nsw i32 %aa48, 8
  store i32 %20, ptr %aa, align 4
  %aa49 = load i32, ptr %aa, align 4
  br label %last42

last42:                                           ; preds = %then41, %mergeBB
  %aa50 = load i32, ptr %aa, align 4
  %21 = shl i32 %aa50, 2
  store i32 %21, ptr %aa, align 4
  %aa51 = load i32, ptr %aa, align 4
  %b52 = load i32, ptr %b, align 4
  %22 = ashr i32 %b52, 1
  store i32 %22, ptr %b, align 4
  %b53 = load i32, ptr %b, align 4
  %aa54 = load i32, ptr %aa, align 4
  %b55 = load i32, ptr %b, align 4
  %23 = and i32 %aa54, %b55
  store i32 %23, ptr %aa, align 4
  %aa56 = load i32, ptr %aa, align 4
  %aa57 = load i32, ptr %aa, align 4
  %b58 = load i32, ptr %b, align 4
  %24 = or i32 %aa57, %b58
  store i32 %24, ptr %aa, align 4
  %aa59 = load i32, ptr %aa, align 4
  %aa60 = load i32, ptr %aa, align 4
  %b61 = load i32, ptr %b, align 4
  %25 = xor i32 %aa60, %b61
  store i32 %25, ptr %aa, align 4
  %aa62 = load i32, ptr %aa, align 4
  %aa63 = load i32, ptr %aa, align 4
  %26 = srem i32 %aa63, 10
  store i32 %26, ptr %aa, align 4
  %aa64 = load i32, ptr %aa, align 4
  %aa65 = load i32, ptr %aa, align 4
  %b66 = load i32, ptr %b, align 4
  %27 = mul nsw i32 %b66, 9
  %28 = add nsw i32 %aa65, %27
  %29 = call i32 (ptr, ...) @printf(ptr @0, i32 %28)
  ret i32 0

nextBB:                                           ; preds = %cond40
  %b46 = load i32, ptr %b, align 4
  %30 = icmp ne i32 %b46, 0
  br i1 %30, label %nextBB44, label %falseBB

trueBB:                                           ; preds = %cond40
  br label %mergeBB

mergeBB:                                          ; preds = %trueBB, %mergeBB45
  %31 = phi i32 [ %37, %mergeBB45 ], [ 1, %trueBB ]
  %32 = icmp ne i32 %31, 0
  br i1 %32, label %then41, label %last42

nextBB44:                                         ; preds = %nextBB
  %b47 = load i32, ptr %b, align 4
  %33 = icmp ne i32 %b47, 0
  %34 = zext i1 %33 to i32
  br label %mergeBB45

falseBB:                                          ; preds = %nextBB
  br label %mergeBB45

mergeBB45:                                        ; preds = %falseBB, %nextBB44
  %35 = phi i32 [ %34, %nextBB44 ], [ 0, %falseBB ]
  %36 = icmp ne i32 %35, 0
  %37 = zext i1 %36 to i32
  br label %mergeBB
}
