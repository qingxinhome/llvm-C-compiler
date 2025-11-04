; ModuleID = 'demo/e13.c'
source_filename = "demo/e13.c"

define i32 @sum(ptr %array, i32 %m, i32 %n) {
entry:
  %j = alloca i32, align 4
  %i = alloca i32, align 4
  %s = alloca i32, align 4
  %array1 = alloca ptr, align 8
  store ptr %array, ptr %array1, align 8
  %m2 = alloca i32, align 4
  store i32 %m, ptr %m2, align 4
  %n3 = alloca i32, align 4
  store i32 %n, ptr %n3, align 4
  store i32 0, ptr %s, align 4
  br label %for.init

for.init:                                         ; preds = %entry
  %i4 = load i32, ptr %i, align 4
  store i32 0, ptr %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc18, %for.init
  %i5 = load i32, ptr %i, align 4
  %m6 = load i32, ptr %m2, align 4
  %0 = icmp slt i32 %i5, %m6
  %1 = sext i1 %0 to i32
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %for.body, label %for.last20

for.body:                                         ; preds = %for.cond
  br label %for.init7

for.init7:                                        ; preds = %for.body
  %j8 = load i32, ptr %j, align 4
  store i32 0, ptr %j, align 4
  br label %for.cond9

for.cond9:                                        ; preds = %for.inc, %for.init7
  %j10 = load i32, ptr %j, align 4
  %n11 = load i32, ptr %n3, align 4
  %3 = icmp slt i32 %j10, %n11
  %4 = sext i1 %3 to i32
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %for.body12, label %for.last

for.body12:                                       ; preds = %for.cond9
  %s13 = load i32, ptr %s, align 4
  %array14 = load ptr, ptr %array1, align 8
  %i15 = load i32, ptr %i, align 4
  %6 = getelementptr inbounds [4 x i32], ptr %array14, i32 %i15
  %7 = load [4 x i32], ptr %6, align 4
  %j16 = load i32, ptr %j, align 4
  %8 = getelementptr inbounds i32, ptr %6, i32 %j16
  %9 = load i32, ptr %8, align 4
  %10 = add i32 %s13, %9
  store i32 %10, ptr %s, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body12
  %j17 = load i32, ptr %j, align 4
  %11 = add i32 %j17, 1
  store i32 %11, ptr %j, align 4
  br label %for.cond9

for.last:                                         ; preds = %for.cond9
  br label %for.inc18

for.inc18:                                        ; preds = %for.last
  %i19 = load i32, ptr %i, align 4
  %12 = add i32 %i19, 1
  store i32 %12, ptr %i, align 4
  br label %for.cond

for.last20:                                       ; preds = %for.cond
  %s21 = load i32, ptr %s, align 4
  ret i32 %s21
}

define i32 @main() {
entry:
  %col = alloca i32, align 4
  %row = alloca i32, align 4
  %a = alloca [4 x [4 x i32]], align 4
  %0 = getelementptr inbounds [4 x [4 x i32]], ptr %a, i32 0, i32 0, i32 0
  store i32 1, ptr %0, align 4
  %1 = getelementptr inbounds [4 x [4 x i32]], ptr %a, i32 0, i32 0, i32 1
  store i32 2, ptr %1, align 4
  %2 = getelementptr inbounds [4 x [4 x i32]], ptr %a, i32 0, i32 0, i32 2
  store i32 3, ptr %2, align 4
  %3 = getelementptr inbounds [4 x [4 x i32]], ptr %a, i32 0, i32 0, i32 3
  store i32 4, ptr %3, align 4
  %4 = getelementptr inbounds [4 x [4 x i32]], ptr %a, i32 0, i32 1, i32 0
  store i32 5, ptr %4, align 4
  %5 = getelementptr inbounds [4 x [4 x i32]], ptr %a, i32 0, i32 1, i32 1
  store i32 6, ptr %5, align 4
  %6 = getelementptr inbounds [4 x [4 x i32]], ptr %a, i32 0, i32 1, i32 2
  store i32 7, ptr %6, align 4
  %7 = getelementptr inbounds [4 x [4 x i32]], ptr %a, i32 0, i32 1, i32 3
  store i32 8, ptr %7, align 4
  %8 = getelementptr inbounds [4 x [4 x i32]], ptr %a, i32 0, i32 2, i32 0
  store i32 9, ptr %8, align 4
  %9 = getelementptr inbounds [4 x [4 x i32]], ptr %a, i32 0, i32 2, i32 1
  store i32 10, ptr %9, align 4
  %10 = getelementptr inbounds [4 x [4 x i32]], ptr %a, i32 0, i32 2, i32 2
  store i32 11, ptr %10, align 4
  %11 = getelementptr inbounds [4 x [4 x i32]], ptr %a, i32 0, i32 2, i32 3
  store i32 12, ptr %11, align 4
  %12 = getelementptr inbounds [4 x [4 x i32]], ptr %a, i32 0, i32 3, i32 0
  store i32 13, ptr %12, align 4
  %13 = getelementptr inbounds [4 x [4 x i32]], ptr %a, i32 0, i32 3, i32 1
  store i32 14, ptr %13, align 4
  %14 = getelementptr inbounds [4 x [4 x i32]], ptr %a, i32 0, i32 3, i32 2
  store i32 15, ptr %14, align 4
  %15 = getelementptr inbounds [4 x [4 x i32]], ptr %a, i32 0, i32 3, i32 3
  store i32 16, ptr %15, align 4
  store i32 3, ptr %row, align 4
  store i32 2, ptr %col, align 4
  %a1 = load [4 x [4 x i32]], ptr %a, align 4
  %row2 = load i32, ptr %row, align 4
  %col3 = load i32, ptr %col, align 4
  %16 = call i32 @sum(ptr %a, i32 %row2, i32 %col3)
  ret i32 %16
}
