; ModuleID = 'demo/e10.c'
source_filename = "demo/e10.c"

@0 = global [5 x i32] zeroinitializer, align 4

define i32 @combine(ptr %arr1, i32 %arr1_length, ptr %arr2, i32 %arr2_length) {
entry:
  %k = alloca i32, align 4
  %j = alloca i32, align 4
  %i = alloca i32, align 4
  %arr11 = alloca ptr, align 8
  store ptr %arr1, ptr %arr11, align 8
  %arr1_length2 = alloca i32, align 4
  store i32 %arr1_length, ptr %arr1_length2, align 4
  %arr23 = alloca ptr, align 8
  store ptr %arr2, ptr %arr23, align 8
  %arr2_length4 = alloca i32, align 4
  store i32 %arr2_length, ptr %arr2_length4, align 4
  store i32 0, ptr %i, align 4
  store i32 0, ptr %j, align 4
  store i32 0, ptr %k, align 4
  br label %for.init

for.init:                                         ; preds = %entry
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %for.init
  %i5 = load i32, ptr %i, align 4
  %arr1_length6 = load i32, ptr %arr1_length2, align 4
  %0 = icmp slt i32 %i5, %arr1_length6
  %1 = sext i1 %0 to i32
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %nextBB, label %falseBB

nextBB:                                           ; preds = %for.cond
  %j7 = load i32, ptr %j, align 4
  %arr2_length8 = load i32, ptr %arr2_length4, align 4
  %3 = icmp slt i32 %j7, %arr2_length8
  %4 = sext i1 %3 to i32
  %5 = icmp ne i32 %4, 0
  %6 = zext i1 %5 to i32
  br label %mergeBB

falseBB:                                          ; preds = %for.cond
  br label %mergeBB

mergeBB:                                          ; preds = %falseBB, %nextBB
  %7 = phi i32 [ %6, %nextBB ], [ 0, %falseBB ]
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %for.body, label %for.last

for.body:                                         ; preds = %mergeBB
  br label %cond

cond:                                             ; preds = %for.body
  %arr19 = load ptr, ptr %arr11, align 8
  %i10 = load i32, ptr %i, align 4
  %9 = getelementptr inbounds i32, ptr %arr19, i32 %i10
  %10 = load i32, ptr %9, align 4
  %arr211 = load ptr, ptr %arr23, align 8
  %j12 = load i32, ptr %j, align 4
  %11 = getelementptr inbounds i32, ptr %arr211, i32 %j12
  %12 = load i32, ptr %11, align 4
  %13 = icmp slt i32 %10, %12
  %14 = sext i1 %13 to i32
  %15 = icmp ne i32 %14, 0
  br i1 %15, label %then, label %else

then:                                             ; preds = %cond
  %sort_arr = load [5 x i32], ptr @0, align 4
  %k13 = load i32, ptr %k, align 4
  %16 = getelementptr inbounds i32, ptr @0, i32 %k13
  %17 = load i32, ptr %16, align 4
  %arr114 = load ptr, ptr %arr11, align 8
  %i15 = load i32, ptr %i, align 4
  %18 = getelementptr inbounds i32, ptr %arr114, i32 %i15
  %19 = load i32, ptr %18, align 4
  store i32 %19, ptr %16, align 4
  %i16 = load i32, ptr %i, align 4
  %i17 = load i32, ptr %i, align 4
  %20 = add nsw i32 %i17, 1
  store i32 %20, ptr %i, align 4
  br label %last

else:                                             ; preds = %cond
  %sort_arr18 = load [5 x i32], ptr @0, align 4
  %k19 = load i32, ptr %k, align 4
  %21 = getelementptr inbounds i32, ptr @0, i32 %k19
  %22 = load i32, ptr %21, align 4
  %arr220 = load ptr, ptr %arr23, align 8
  %j21 = load i32, ptr %j, align 4
  %23 = getelementptr inbounds i32, ptr %arr220, i32 %j21
  %24 = load i32, ptr %23, align 4
  store i32 %24, ptr %21, align 4
  %j22 = load i32, ptr %j, align 4
  %j23 = load i32, ptr %j, align 4
  %25 = add nsw i32 %j23, 1
  store i32 %25, ptr %j, align 4
  br label %last

last:                                             ; preds = %else, %then
  %k24 = load i32, ptr %k, align 4
  %k25 = load i32, ptr %k, align 4
  %26 = add nsw i32 %k25, 1
  store i32 %26, ptr %k, align 4
  br label %for.inc

for.inc:                                          ; preds = %last
  br label %for.cond

for.last:                                         ; preds = %mergeBB
  br label %cond26

cond26:                                           ; preds = %for.last
  %i27 = load i32, ptr %i, align 4
  %arr1_length28 = load i32, ptr %arr1_length2, align 4
  %27 = icmp eq i32 %i27, %arr1_length28
  %28 = sext i1 %27 to i32
  %29 = icmp ne i32 %28, 0
  br i1 %29, label %then29, label %else45

then29:                                           ; preds = %cond26
  br label %for.init30

for.init30:                                       ; preds = %then29
  br label %for.cond31

for.cond31:                                       ; preds = %for.inc43, %for.init30
  %j32 = load i32, ptr %j, align 4
  %arr2_length33 = load i32, ptr %arr2_length4, align 4
  %30 = icmp slt i32 %j32, %arr2_length33
  %31 = sext i1 %30 to i32
  %32 = icmp ne i32 %31, 0
  br i1 %32, label %for.body34, label %for.last44

for.body34:                                       ; preds = %for.cond31
  %sort_arr35 = load [5 x i32], ptr @0, align 4
  %k36 = load i32, ptr %k, align 4
  %33 = getelementptr inbounds i32, ptr @0, i32 %k36
  %34 = load i32, ptr %33, align 4
  %arr237 = load ptr, ptr %arr23, align 8
  %j38 = load i32, ptr %j, align 4
  %35 = getelementptr inbounds i32, ptr %arr237, i32 %j38
  %36 = load i32, ptr %35, align 4
  store i32 %36, ptr %33, align 4
  %k39 = load i32, ptr %k, align 4
  %k40 = load i32, ptr %k, align 4
  %37 = add nsw i32 %k40, 1
  store i32 %37, ptr %k, align 4
  %j41 = load i32, ptr %j, align 4
  %j42 = load i32, ptr %j, align 4
  %38 = add nsw i32 %j42, 1
  store i32 %38, ptr %j, align 4
  br label %for.inc43

for.inc43:                                        ; preds = %for.body34
  br label %for.cond31

for.last44:                                       ; preds = %for.cond31
  br label %last61

else45:                                           ; preds = %cond26
  br label %for.init46

for.init46:                                       ; preds = %else45
  br label %for.cond47

for.cond47:                                       ; preds = %for.inc59, %for.init46
  %i48 = load i32, ptr %i, align 4
  %arr1_length49 = load i32, ptr %arr1_length2, align 4
  %39 = icmp slt i32 %i48, %arr1_length49
  %40 = sext i1 %39 to i32
  %41 = icmp ne i32 %40, 0
  br i1 %41, label %for.body50, label %for.last60

for.body50:                                       ; preds = %for.cond47
  %sort_arr51 = load [5 x i32], ptr @0, align 4
  %k52 = load i32, ptr %k, align 4
  %42 = getelementptr inbounds i32, ptr @0, i32 %k52
  %43 = load i32, ptr %42, align 4
  %arr253 = load ptr, ptr %arr23, align 8
  %i54 = load i32, ptr %i, align 4
  %44 = getelementptr inbounds i32, ptr %arr253, i32 %i54
  %45 = load i32, ptr %44, align 4
  store i32 %45, ptr %42, align 4
  %k55 = load i32, ptr %k, align 4
  %k56 = load i32, ptr %k, align 4
  %46 = add nsw i32 %k56, 1
  store i32 %46, ptr %k, align 4
  %i57 = load i32, ptr %i, align 4
  %i58 = load i32, ptr %i, align 4
  %47 = add nsw i32 %i58, 1
  store i32 %47, ptr %i, align 4
  br label %for.inc59

for.inc59:                                        ; preds = %for.body50
  br label %for.cond47

for.last60:                                       ; preds = %for.cond47
  br label %last61

last61:                                           ; preds = %for.last60, %for.last44
  %sort_arr62 = load [5 x i32], ptr @0, align 4
  %arr1_length63 = load i32, ptr %arr1_length2, align 4
  %arr2_length64 = load i32, ptr %arr2_length4, align 4
  %48 = add nsw i32 %arr1_length63, %arr2_length64
  %49 = sub nsw i32 %48, 1
  %50 = getelementptr inbounds i32, ptr @0, i32 %49
  %51 = load i32, ptr %50, align 4
  ret i32 %51
}

define i32 @main() {
entry:
  %b = alloca [3 x i32], align 4
  %a = alloca [2 x i32], align 4
  %0 = getelementptr inbounds [2 x i32], ptr %a, i32 0, i32 0
  store i32 1, ptr %0, align 4
  %1 = getelementptr inbounds [2 x i32], ptr %a, i32 0, i32 1
  store i32 5, ptr %1, align 4
  %2 = getelementptr inbounds [3 x i32], ptr %b, i32 0, i32 0
  store i32 1, ptr %2, align 4
  %3 = getelementptr inbounds [3 x i32], ptr %b, i32 0, i32 1
  store i32 4, ptr %3, align 4
  %4 = getelementptr inbounds [3 x i32], ptr %b, i32 0, i32 2
  store i32 14, ptr %4, align 4
  %a1 = load [2 x i32], ptr %a, align 4
  %b2 = load [3 x i32], ptr %b, align 4
  %5 = call i32 @combine(ptr %a, i32 2, ptr %b, i32 3)
  ret i32 %5
}
