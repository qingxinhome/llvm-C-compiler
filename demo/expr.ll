; ModuleID = 'expr'
source_filename = "expr"

@0 = private unnamed_addr constant [18 x i8] c"expr value: [%d]\0A\00", align 1

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
  %b11 = load i32, ptr %b, align 4
  store i32 19, ptr %b, align 4
  br label %last

last:                                             ; preds = %else, %last8
  br label %for.init

cond5:                                            ; preds = %then
  br i1 false, label %then6, label %else7

then6:                                            ; preds = %cond5
  %b9 = load i32, ptr %b, align 4
  store i32 9, ptr %b, align 4
  br label %last8

else7:                                            ; preds = %cond5
  %aa10 = load i32, ptr %aa, align 4
  store i32 30, ptr %aa, align 4
  br label %last8

last8:                                            ; preds = %else7, %then6
  br label %last

for.init:                                         ; preds = %last
  %i = alloca i32, align 4
  store i32 0, ptr %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %for.init
  %i12 = load i32, ptr %i, align 4
  %3 = icmp slt i32 %i12, 100
  %4 = sext i1 %3 to i32
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %for.body, label %for.last

for.inc:                                          ; preds = %for.last24
  %i35 = load i32, ptr %i, align 4
  %i36 = load i32, ptr %i, align 4
  %6 = add nsw i32 %i36, 1
  store i32 %6, ptr %i, align 4
  br label %for.cond

for.body:                                         ; preds = %for.cond
  %aa13 = load i32, ptr %aa, align 4
  %aa14 = load i32, ptr %aa, align 4
  %i15 = load i32, ptr %i, align 4
  %7 = add nsw i32 %aa14, %i15
  store i32 %7, ptr %aa, align 4
  br label %cond16

for.last:                                         ; preds = %then17, %for.cond
  br label %cond37

cond16:                                           ; preds = %for.body
  %i19 = load i32, ptr %i, align 4
  %8 = icmp sge i32 %i19, 55
  %9 = sext i1 %8 to i32
  %10 = icmp ne i32 %9, 0
  br i1 %10, label %then17, label %last18

then17:                                           ; preds = %cond16
  br label %for.last

last18:                                           ; preds = %for.break.death, %cond16
  br label %for.init20

for.break.death:                                  ; No predecessors!
  br label %last18

for.init20:                                       ; preds = %last18
  %j = alloca i32, align 4
  store i32 1, ptr %j, align 4
  br label %for.cond21

for.cond21:                                       ; preds = %for.inc22, %for.init20
  %j25 = load i32, ptr %j, align 4
  %11 = icmp slt i32 %j25, 10
  %12 = sext i1 %11 to i32
  %13 = icmp ne i32 %12, 0
  br i1 %13, label %for.body23, label %for.last24

for.inc22:                                        ; preds = %last28, %then27
  %j33 = load i32, ptr %j, align 4
  %j34 = load i32, ptr %j, align 4
  %14 = add nsw i32 %j34, 2
  store i32 %14, ptr %j, align 4
  br label %for.cond21

for.body23:                                       ; preds = %for.cond21
  br label %cond26

for.last24:                                       ; preds = %for.cond21
  br label %for.inc

cond26:                                           ; preds = %for.body23
  %i29 = load i32, ptr %i, align 4
  %15 = icmp eq i32 %i29, 10
  %16 = sext i1 %15 to i32
  %17 = icmp ne i32 %16, 0
  br i1 %17, label %then27, label %last28

then27:                                           ; preds = %cond26
  br label %for.inc22

last28:                                           ; preds = %for.continue.death, %cond26
  %b30 = load i32, ptr %b, align 4
  %b31 = load i32, ptr %b, align 4
  %j32 = load i32, ptr %j, align 4
  %18 = mul nsw i32 %b31, %j32
  store i32 %18, ptr %b, align 4
  br label %for.inc22

for.continue.death:                               ; No predecessors!
  br label %last28

cond37:                                           ; preds = %for.last
  %aa40 = load i32, ptr %aa, align 4
  %19 = icmp ne i32 %aa40, 0
  br i1 %19, label %trueBB, label %nextBB

then38:                                           ; preds = %mergeBB
  %aa45 = load i32, ptr %aa, align 4
  %aa46 = load i32, ptr %aa, align 4
  %20 = add nsw i32 %aa46, 8
  store i32 %20, ptr %aa, align 4
  br label %last39

last39:                                           ; preds = %then38, %mergeBB
  %aa47 = load i32, ptr %aa, align 4
  %aa48 = load i32, ptr %aa, align 4
  %21 = shl i32 %aa48, 2
  store i32 %21, ptr %aa, align 4
  %b49 = load i32, ptr %b, align 4
  %b50 = load i32, ptr %b, align 4
  %22 = ashr i32 %b50, 1
  store i32 %22, ptr %b, align 4
  %aa51 = load i32, ptr %aa, align 4
  %aa52 = load i32, ptr %aa, align 4
  %b53 = load i32, ptr %b, align 4
  %23 = and i32 %aa52, %b53
  store i32 %23, ptr %aa, align 4
  %aa54 = load i32, ptr %aa, align 4
  %aa55 = load i32, ptr %aa, align 4
  %b56 = load i32, ptr %b, align 4
  %24 = or i32 %aa55, %b56
  store i32 %24, ptr %aa, align 4
  %aa57 = load i32, ptr %aa, align 4
  %aa58 = load i32, ptr %aa, align 4
  %b59 = load i32, ptr %b, align 4
  %25 = xor i32 %aa58, %b59
  store i32 %25, ptr %aa, align 4
  %aa60 = load i32, ptr %aa, align 4
  %aa61 = load i32, ptr %aa, align 4
  %26 = srem i32 %aa61, 10
  store i32 %26, ptr %aa, align 4
  %aa62 = load i32, ptr %aa, align 4
  %b63 = load i32, ptr %b, align 4
  %27 = mul nsw i32 %b63, 9
  %28 = add nsw i32 %aa62, %27
  %29 = call i32 (ptr, ...) @printf(ptr @0, i32 %28)
  ret i32 0

nextBB:                                           ; preds = %cond37
  %b43 = load i32, ptr %b, align 4
  %30 = icmp ne i32 %b43, 0
  br i1 %30, label %nextBB41, label %falseBB

trueBB:                                           ; preds = %cond37
  br label %mergeBB

mergeBB:                                          ; preds = %trueBB, %mergeBB42
  %31 = phi i32 [ %37, %mergeBB42 ], [ 1, %trueBB ]
  %32 = icmp ne i32 %31, 0
  br i1 %32, label %then38, label %last39

nextBB41:                                         ; preds = %nextBB
  %b44 = load i32, ptr %b, align 4
  %33 = icmp ne i32 %b44, 0
  %34 = zext i1 %33 to i32
  br label %mergeBB42

falseBB:                                          ; preds = %nextBB
  br label %mergeBB42

mergeBB42:                                        ; preds = %falseBB, %nextBB41
  %35 = phi i32 [ %34, %nextBB41 ], [ 0, %falseBB ]
  %36 = icmp ne i32 %35, 0
  %37 = zext i1 %36 to i32
  br label %mergeBB
}
