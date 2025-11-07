; ModuleID = 'demo/nqueen.c'
source_filename = "demo/nqueen.c"

@"Q " = private constant [3 x i8] c"Q \00"
@". " = private constant [3 x i8] c". \00"
@"\0A" = private constant [2 x i8] c"\0A\00"
@"\0A\0A" = private constant [3 x i8] c"\0A\0A\00"

declare i32 @printf(ptr, ...)

define void @print_board(ptr %board) {
entry:
  %j = alloca i32, align 4
  %i = alloca i32, align 4
  %board1 = alloca ptr, align 8
  store ptr %board, ptr %board1, align 8
  store i32 0, ptr %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc10, %entry
  %i2 = load i32, ptr %i, align 4
  %0 = icmp slt i32 %i2, 10
  %1 = sext i1 %0 to i32
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %for.body, label %for.last12

for.body:                                         ; preds = %for.cond
  store i32 0, ptr %j, align 4
  br label %for.cond3

for.cond3:                                        ; preds = %for.inc, %for.body
  %j4 = load i32, ptr %j, align 4
  %3 = icmp slt i32 %j4, 10
  %4 = sext i1 %3 to i32
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %for.body5, label %for.last

for.body5:                                        ; preds = %for.cond3
  %board6 = load ptr, ptr %board1, align 8
  %i7 = load i32, ptr %i, align 4
  %6 = getelementptr inbounds [10 x i32], ptr %board6, i32 %i7
  %7 = load [10 x i32], ptr %6, align 4
  %j8 = load i32, ptr %j, align 4
  %8 = getelementptr inbounds i32, ptr %6, i32 %j8
  %9 = load i32, ptr %8, align 4
  %10 = icmp ne i32 %9, 0
  br i1 %10, label %then, label %else

then:                                             ; preds = %for.body5
  %11 = call i32 (ptr, ...) @printf(ptr @"Q ")
  br label %last

else:                                             ; preds = %for.body5
  %12 = call i32 (ptr, ...) @printf(ptr @". ")
  br label %last

last:                                             ; preds = %else, %then
  br label %for.inc

for.inc:                                          ; preds = %last
  %j9 = load i32, ptr %j, align 4
  %13 = add i32 %j9, 1
  store i32 %13, ptr %j, align 4
  br label %for.cond3

for.last:                                         ; preds = %for.cond3
  %14 = call i32 (ptr, ...) @printf(ptr @"\0A")
  br label %for.inc10

for.inc10:                                        ; preds = %for.last
  %i11 = load i32, ptr %i, align 4
  %15 = add i32 %i11, 1
  store i32 %15, ptr %i, align 4
  br label %for.cond

for.last12:                                       ; preds = %for.cond
  %16 = call i32 (ptr, ...) @printf(ptr @"\0A\0A")
  ret void
}

define i32 @conflict(ptr %board, i32 %row, i32 %col) {
entry:
  %j = alloca i32, align 4
  %i = alloca i32, align 4
  %board1 = alloca ptr, align 8
  store ptr %board, ptr %board1, align 8
  %row2 = alloca i32, align 4
  store i32 %row, ptr %row2, align 4
  %col3 = alloca i32, align 4
  store i32 %col, ptr %col3, align 4
  store i32 0, ptr %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %i4 = load i32, ptr %i, align 4
  %row5 = load i32, ptr %row2, align 4
  %0 = icmp slt i32 %i4, %row5
  %1 = sext i1 %0 to i32
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %for.body, label %for.last

for.body:                                         ; preds = %for.cond
  %board6 = load ptr, ptr %board1, align 8
  %i7 = load i32, ptr %i, align 4
  %3 = getelementptr inbounds [10 x i32], ptr %board6, i32 %i7
  %4 = load [10 x i32], ptr %3, align 4
  %col8 = load i32, ptr %col3, align 4
  %5 = getelementptr inbounds i32, ptr %3, i32 %col8
  %6 = load i32, ptr %5, align 4
  %7 = icmp ne i32 %6, 0
  br i1 %7, label %then, label %last

then:                                             ; preds = %for.body
  ret i32 1

last:                                             ; preds = %for.body
  %row9 = load i32, ptr %row2, align 4
  %i10 = load i32, ptr %i, align 4
  %8 = sub nsw i32 %row9, %i10
  store i32 %8, ptr %j, align 4
  %col11 = load i32, ptr %col3, align 4
  %j12 = load i32, ptr %j, align 4
  %9 = sub nsw i32 %col11, %j12
  %10 = add nsw i32 %9, 1
  %11 = icmp slt i32 0, %10
  %12 = sext i1 %11 to i32
  %13 = icmp ne i32 %12, 0
  br i1 %13, label %nextBB, label %falseBB

nextBB:                                           ; preds = %last
  %board13 = load ptr, ptr %board1, align 8
  %i14 = load i32, ptr %i, align 4
  %14 = getelementptr inbounds [10 x i32], ptr %board13, i32 %i14
  %15 = load [10 x i32], ptr %14, align 4
  %col15 = load i32, ptr %col3, align 4
  %j16 = load i32, ptr %j, align 4
  %16 = sub nsw i32 %col15, %j16
  %17 = getelementptr inbounds i32, ptr %14, i32 %16
  %18 = load i32, ptr %17, align 4
  %19 = icmp ne i32 %18, 0
  %20 = zext i1 %19 to i32
  br label %mergeBB

falseBB:                                          ; preds = %last
  br label %mergeBB

mergeBB:                                          ; preds = %falseBB, %nextBB
  %21 = phi i32 [ %20, %nextBB ], [ 0, %falseBB ]
  %22 = icmp ne i32 %21, 0
  br i1 %22, label %then17, label %last18

then17:                                           ; preds = %mergeBB
  ret i32 1

last18:                                           ; preds = %mergeBB
  %col19 = load i32, ptr %col3, align 4
  %j20 = load i32, ptr %j, align 4
  %23 = add nsw i32 %col19, %j20
  %24 = icmp slt i32 %23, 10
  %25 = sext i1 %24 to i32
  %26 = icmp ne i32 %25, 0
  br i1 %26, label %nextBB21, label %falseBB26

nextBB21:                                         ; preds = %last18
  %board22 = load ptr, ptr %board1, align 8
  %i23 = load i32, ptr %i, align 4
  %27 = getelementptr inbounds [10 x i32], ptr %board22, i32 %i23
  %28 = load [10 x i32], ptr %27, align 4
  %col24 = load i32, ptr %col3, align 4
  %j25 = load i32, ptr %j, align 4
  %29 = add nsw i32 %col24, %j25
  %30 = getelementptr inbounds i32, ptr %27, i32 %29
  %31 = load i32, ptr %30, align 4
  %32 = icmp ne i32 %31, 0
  %33 = zext i1 %32 to i32
  br label %mergeBB27

falseBB26:                                        ; preds = %last18
  br label %mergeBB27

mergeBB27:                                        ; preds = %falseBB26, %nextBB21
  %34 = phi i32 [ %33, %nextBB21 ], [ 0, %falseBB26 ]
  %35 = icmp ne i32 %34, 0
  br i1 %35, label %then28, label %last29

then28:                                           ; preds = %mergeBB27
  ret i32 1

last29:                                           ; preds = %mergeBB27
  br label %for.inc

for.inc:                                          ; preds = %last29
  %i30 = load i32, ptr %i, align 4
  %36 = add i32 %i30, 1
  store i32 %36, ptr %i, align 4
  br label %for.cond

for.last:                                         ; preds = %for.cond
  ret i32 0
}

define void @solve(ptr %board, i32 %row) {
entry:
  %i = alloca i32, align 4
  %board1 = alloca ptr, align 8
  store ptr %board, ptr %board1, align 8
  %row2 = alloca i32, align 4
  store i32 %row, ptr %row2, align 4
  %row3 = load i32, ptr %row2, align 4
  %0 = icmp sgt i32 %row3, 9
  %1 = sext i1 %0 to i32
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %then, label %last

then:                                             ; preds = %entry
  %board4 = load ptr, ptr %board1, align 8
  call void @print_board(ptr %board4)
  br label %last

last:                                             ; preds = %then, %entry
  store i32 0, ptr %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %last
  %i5 = load i32, ptr %i, align 4
  %3 = icmp slt i32 %i5, 10
  %4 = sext i1 %3 to i32
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %for.body, label %for.last

for.body:                                         ; preds = %for.cond
  %board6 = load ptr, ptr %board1, align 8
  %row7 = load i32, ptr %row2, align 4
  %i8 = load i32, ptr %i, align 4
  %6 = call i32 @conflict(ptr %board6, i32 %row7, i32 %i8)
  %7 = icmp ne i32 %6, 0
  br i1 %7, label %then9, label %else

then9:                                            ; preds = %for.body
  br label %last18

else:                                             ; preds = %for.body
  %board10 = load ptr, ptr %board1, align 8
  %row11 = load i32, ptr %row2, align 4
  %8 = getelementptr inbounds [10 x i32], ptr %board10, i32 %row11
  %9 = load [10 x i32], ptr %8, align 4
  %i12 = load i32, ptr %i, align 4
  %10 = getelementptr inbounds i32, ptr %8, i32 %i12
  %11 = load i32, ptr %10, align 4
  store i32 1, ptr %10, align 4
  %board13 = load ptr, ptr %board1, align 8
  %row14 = load i32, ptr %row2, align 4
  %12 = add nsw i32 %row14, 1
  call void @solve(ptr %board13, i32 %12)
  %board15 = load ptr, ptr %board1, align 8
  %row16 = load i32, ptr %row2, align 4
  %13 = getelementptr inbounds [10 x i32], ptr %board15, i32 %row16
  %14 = load [10 x i32], ptr %13, align 4
  %i17 = load i32, ptr %i, align 4
  %15 = getelementptr inbounds i32, ptr %13, i32 %i17
  %16 = load i32, ptr %15, align 4
  store i32 0, ptr %15, align 4
  br label %last18

last18:                                           ; preds = %else, %then9
  br label %for.inc

for.inc:                                          ; preds = %last18
  %i19 = load i32, ptr %i, align 4
  %17 = add i32 %i19, 1
  store i32 %17, ptr %i, align 4
  br label %for.cond

for.last:                                         ; preds = %for.cond
  ret void
}

define i32 @main() {
entry:
  %i = alloca i32, align 4
  %board = alloca [100 x i32], align 4
  store i32 0, ptr %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %i1 = load i32, ptr %i, align 4
  %0 = icmp slt i32 %i1, 100
  %1 = sext i1 %0 to i32
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %for.body, label %for.last

for.body:                                         ; preds = %for.cond
  %board2 = load [100 x i32], ptr %board, align 4
  %i3 = load i32, ptr %i, align 4
  %3 = getelementptr inbounds i32, ptr %board, i32 %i3
  %4 = load i32, ptr %3, align 4
  store i32 0, ptr %3, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %i4 = load i32, ptr %i, align 4
  %5 = add i32 %i4, 1
  store i32 %5, ptr %i, align 4
  br label %for.cond

for.last:                                         ; preds = %for.cond
  %board5 = load [100 x i32], ptr %board, align 4
  call void @solve(ptr %board, i32 0)
  ret i32 0
}
