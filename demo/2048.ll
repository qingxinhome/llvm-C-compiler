; ModuleID = 'demo/2048.c'
source_filename = "demo/2048.c"

@"%d" = private constant [3 x i8] c"%d\00"
@"%d.1" = private constant [3 x i8] c"%d\00"
@"%d.2" = private constant [3 x i8] c"%d\00"
@"%d:" = private constant [4 x i8] c"%d:\00"
@" %d" = private constant [4 x i8] c" %d\00"
@0 = global i32 0, align 4
@1 = global i32 1, align 4
@2 = global i32 2, align 4
@3 = global i32 3, align 4
@4 = global i32 4, align 4
@5 = global [20 x i32] [i32 1, i32 2, i32 4, i32 8, i32 16, i32 32, i32 64, i32 128, i32 256, i32 512, i32 1024, i32 2048, i32 4096, i32 8192, i32 16384, i32 32768, i32 65536, i32 131072, i32 262144, i32 524288], align 4
@6 = global [20 x i32] [i32 0, i32 1, i32 1, i32 1, i32 2, i32 2, i32 2, i32 3, i32 3, i32 3, i32 4, i32 4, i32 4, i32 4, i32 5, i32 5, i32 5, i32 6, i32 6, i32 6], align 4
@7 = global [25 x i32] [i32 73, i32 110, i32 112, i32 117, i32 116, i32 32, i32 97, i32 32, i32 114, i32 97, i32 110, i32 100, i32 111, i32 109, i32 32, i32 110, i32 117, i32 109, i32 98, i32 101, i32 114, i32 58, i32 32, i32 10, i32 0], align 4
@8 = global [62 x i32] [i32 119, i32 44, i32 32, i32 97, i32 44, i32 32, i32 115, i32 44, i32 32, i32 100, i32 58, i32 32, i32 109, i32 111, i32 118, i32 101, i32 10, i32 104, i32 58, i32 32, i32 112, i32 114, i32 105, i32 110, i32 116, i32 32, i32 116, i32 104, i32 105, i32 115, i32 32, i32 104, i32 101, i32 108, i32 112, i32 10, i32 113, i32 58, i32 32, i32 113, i32 117, i32 105, i32 116, i32 10, i32 112, i32 58, i32 32, i32 112, i32 114, i32 105, i32 110, i32 116, i32 32, i32 116, i32 104, i32 101, i32 32, i32 109, i32 97, i32 112, i32 10, i32 0], align 4
@9 = global [8 x i32] [i32 115, i32 99, i32 111, i32 114, i32 101, i32 58, i32 32, i32 0], align 4
@10 = global [7 x i32] [i32 115, i32 116, i32 101, i32 112, i32 58, i32 32, i32 0], align 4
@11 = global [12 x i32] [i32 71, i32 97, i32 109, i32 101, i32 32, i32 111, i32 118, i32 101, i32 114, i32 46, i32 10, i32 0], align 4
@12 = global [26 x i32] [i32 73, i32 110, i32 118, i32 97, i32 108, i32 105, i32 100, i32 32, i32 105, i32 110, i32 112, i32 117, i32 116, i32 46, i32 32, i32 84, i32 114, i32 121, i32 32, i32 97, i32 103, i32 97, i32 105, i32 110, i32 46, i32 0], align 4
@13 = global i32 32, align 4
@14 = global i32 10, align 4
@15 = global [4 x [4 x i32]] zeroinitializer, align 4
@16 = global i32 0, align 4
@17 = global i32 0, align 4
@18 = global i32 0, align 4
@19 = global i32 0, align 4
@20 = global i32 0, align 4

declare i32 @printf(ptr, ...)

declare i32 @scanf(ptr, ...)

declare i32 @getchar()

declare i32 @putchar(i32)

define i32 @getint() {
entry:
  %val = alloca i32, align 4
  %val1 = load i32, ptr %val, align 4
  %0 = call i32 (ptr, ...) @scanf(ptr @"%d", ptr %val)
  %val2 = load i32, ptr %val, align 4
  ret i32 %val2
}

define i32 @getch() {
entry:
  %0 = call i32 @getchar()
  ret i32 %0
}

define i32 @getarray(ptr %val) {
entry:
  %i = alloca i32, align 4
  %len = alloca i32, align 4
  %val1 = alloca ptr, align 8
  store ptr %val, ptr %val1, align 8
  store i32 0, ptr %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %i2 = load i32, ptr %i, align 4
  %len3 = load i32, ptr %len, align 4
  %0 = icmp slt i32 %i2, %len3
  %1 = sext i1 %0 to i32
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %for.body, label %for.last

for.body:                                         ; preds = %for.cond
  %val4 = load ptr, ptr %val1, align 8
  %i5 = load i32, ptr %i, align 4
  %3 = getelementptr inbounds i32, ptr %val4, i32 %i5
  %4 = load i32, ptr %3, align 4
  %5 = call i32 (ptr, ...) @scanf(ptr @"%d.1", i32 %4)
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %i6 = load i32, ptr %i, align 4
  %6 = add i32 %i6, 1
  store i32 %6, ptr %i, align 4
  br label %for.cond

for.last:                                         ; preds = %for.cond
  %len7 = load i32, ptr %len, align 4
  ret i32 %len7
}

define void @putint(i32 %val) {
entry:
  %val1 = alloca i32, align 4
  store i32 %val, ptr %val1, align 4
  %val2 = load i32, ptr %val1, align 4
  %0 = call i32 (ptr, ...) @printf(ptr @"%d.2", i32 %val2)
  ret void
}

define void @putch(i32 %val) {
entry:
  %val1 = alloca i32, align 4
  store i32 %val, ptr %val1, align 4
  %val2 = load i32, ptr %val1, align 4
  %0 = call i32 @putchar(i32 %val2)
  ret void
}

define void @putarray(i32 %len, ptr %arr) {
entry:
  %i = alloca i32, align 4
  %len1 = alloca i32, align 4
  store i32 %len, ptr %len1, align 4
  %arr2 = alloca ptr, align 8
  store ptr %arr, ptr %arr2, align 8
  %len3 = load i32, ptr %len1, align 4
  %0 = call i32 (ptr, ...) @printf(ptr @"%d:", i32 %len3)
  store i32 0, ptr %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %i4 = load i32, ptr %i, align 4
  %len5 = load i32, ptr %len1, align 4
  %1 = icmp slt i32 %i4, %len5
  %2 = sext i1 %1 to i32
  %3 = icmp ne i32 %2, 0
  br i1 %3, label %for.body, label %for.last

for.body:                                         ; preds = %for.cond
  %arr6 = load ptr, ptr %arr2, align 8
  %i7 = load i32, ptr %i, align 4
  %4 = getelementptr inbounds i32, ptr %arr6, i32 %i7
  %5 = load i32, ptr %4, align 4
  %6 = call i32 (ptr, ...) @printf(ptr @" %d", i32 %5)
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %i8 = load i32, ptr %i, align 4
  %7 = add i32 %i8, 1
  store i32 %7, ptr %i, align 4
  br label %for.cond

for.last:                                         ; preds = %for.cond
  ret void
}

define i32 @rand() {
entry:
  %seed = load i32, ptr @20, align 4
  %seed1 = load i32, ptr @20, align 4
  %0 = mul nsw i32 %seed1, 214013
  %1 = add nsw i32 %0, 2531011
  %2 = srem i32 %1, 1073741824
  store i32 %2, ptr @20, align 4
  %seed2 = load i32, ptr @20, align 4
  %3 = icmp slt i32 %seed2, 0
  %4 = sext i1 %3 to i32
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %then, label %last

then:                                             ; preds = %entry
  %seed3 = load i32, ptr @20, align 4
  %seed4 = load i32, ptr @20, align 4
  %6 = sub i32 0, %seed4
  store i32 %6, ptr @20, align 4
  br label %last

last:                                             ; preds = %then, %entry
  %seed5 = load i32, ptr @20, align 4
  %7 = sdiv i32 %seed5, 65536
  %8 = srem i32 %7, 32768
  ret i32 %8
}

define void @put_string(ptr %str) {
entry:
  %i = alloca i32, align 4
  %str1 = alloca ptr, align 8
  store ptr %str, ptr %str1, align 8
  store i32 0, ptr %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %str2 = load ptr, ptr %str1, align 8
  %i3 = load i32, ptr %i, align 4
  %0 = getelementptr inbounds i32, ptr %str2, i32 %i3
  %1 = load i32, ptr %0, align 4
  %2 = icmp ne i32 %1, 0
  %3 = sext i1 %2 to i32
  %4 = icmp ne i32 %3, 0
  br i1 %4, label %for.body, label %for.last

for.body:                                         ; preds = %for.cond
  %str4 = load ptr, ptr %str1, align 8
  %i5 = load i32, ptr %i, align 4
  %5 = getelementptr inbounds i32, ptr %str4, i32 %i5
  %6 = load i32, ptr %5, align 4
  call void @putch(i32 %6)
  %i6 = load i32, ptr %i, align 4
  %i7 = load i32, ptr %i, align 4
  %7 = add nsw i32 %i7, 1
  store i32 %7, ptr %i, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body
  br label %for.cond

for.last:                                         ; preds = %for.cond
  ret void
}

define void @clear_map() {
entry:
  %y = alloca i32, align 4
  %x = alloca i32, align 4
  store i32 0, ptr %x, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc13, %entry
  %x1 = load i32, ptr %x, align 4
  %MAP_LEN = load i32, ptr @4, align 4
  %0 = icmp slt i32 %x1, %MAP_LEN
  %1 = sext i1 %0 to i32
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %for.body, label %for.last14

for.body:                                         ; preds = %for.cond
  %y2 = load i32, ptr %y, align 4
  store i32 0, ptr %y, align 4
  br label %for.cond3

for.cond3:                                        ; preds = %for.inc, %for.body
  %y4 = load i32, ptr %y, align 4
  %MAP_LEN5 = load i32, ptr @4, align 4
  %3 = icmp slt i32 %y4, %MAP_LEN5
  %4 = sext i1 %3 to i32
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %for.body6, label %for.last

for.body6:                                        ; preds = %for.cond3
  %map = load [4 x [4 x i32]], ptr @15, align 4
  %x7 = load i32, ptr %x, align 4
  %6 = getelementptr inbounds [4 x i32], ptr @15, i32 %x7
  %7 = load [4 x i32], ptr %6, align 4
  %y8 = load i32, ptr %y, align 4
  %8 = getelementptr inbounds i32, ptr %6, i32 %y8
  %9 = load i32, ptr %8, align 4
  store i32 0, ptr %8, align 4
  %y9 = load i32, ptr %y, align 4
  %y10 = load i32, ptr %y, align 4
  %10 = add nsw i32 %y10, 1
  store i32 %10, ptr %y, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body6
  br label %for.cond3

for.last:                                         ; preds = %for.cond3
  %x11 = load i32, ptr %x, align 4
  %x12 = load i32, ptr %x, align 4
  %11 = add nsw i32 %x12, 1
  store i32 %11, ptr %x, align 4
  br label %for.inc13

for.inc13:                                        ; preds = %for.last
  br label %for.cond

for.last14:                                       ; preds = %for.cond
  ret void
}

define void @init() {
entry:
  call void @clear_map()
  %score = load i32, ptr @16, align 4
  store i32 0, ptr @16, align 4
  %step = load i32, ptr @17, align 4
  store i32 0, ptr @17, align 4
  %max_num_len = load i32, ptr @18, align 4
  store i32 1, ptr @18, align 4
  %alive = load i32, ptr @19, align 4
  store i32 1, ptr @19, align 4
  ret void
}

define void @print_map() {
entry:
  %i22 = alloca i32, align 4
  %i = alloca i32, align 4
  %y = alloca i32, align 4
  %x = alloca i32, align 4
  %CHR_ENTER = load i32, ptr @14, align 4
  call void @putch(i32 %CHR_ENTER)
  %STR_STEP = load [7 x i32], ptr @10, align 4
  call void @put_string(ptr @10)
  %step = load i32, ptr @17, align 4
  call void @putint(i32 %step)
  %CHR_ENTER1 = load i32, ptr @14, align 4
  call void @putch(i32 %CHR_ENTER1)
  %STR_SCORE = load [8 x i32], ptr @9, align 4
  call void @put_string(ptr @9)
  %score = load i32, ptr @16, align 4
  call void @putint(i32 %score)
  %CHR_ENTER2 = load i32, ptr @14, align 4
  call void @putch(i32 %CHR_ENTER2)
  store i32 0, ptr %x, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc43, %entry
  %x3 = load i32, ptr %x, align 4
  %MAP_LEN = load i32, ptr @4, align 4
  %0 = icmp slt i32 %x3, %MAP_LEN
  %1 = sext i1 %0 to i32
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %for.body, label %for.last44

for.body:                                         ; preds = %for.cond
  %y4 = load i32, ptr %y, align 4
  store i32 0, ptr %y, align 4
  br label %for.cond5

for.cond5:                                        ; preds = %for.inc38, %for.body
  %y6 = load i32, ptr %y, align 4
  %MAP_LEN7 = load i32, ptr @4, align 4
  %3 = icmp slt i32 %y6, %MAP_LEN7
  %4 = sext i1 %3 to i32
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %for.body8, label %for.last39

for.body8:                                        ; preds = %for.cond5
  %map = load [4 x [4 x i32]], ptr @15, align 4
  %x9 = load i32, ptr %x, align 4
  %6 = getelementptr inbounds [4 x i32], ptr @15, i32 %x9
  %7 = load [4 x i32], ptr %6, align 4
  %y10 = load i32, ptr %y, align 4
  %8 = getelementptr inbounds i32, ptr %6, i32 %y10
  %9 = load i32, ptr %8, align 4
  %10 = icmp eq i32 %9, 0
  %11 = sext i1 %10 to i32
  %12 = icmp ne i32 %11, 0
  br i1 %12, label %then, label %else

then:                                             ; preds = %for.body8
  %LEN_OF_POW2 = load [20 x i32], ptr @6, align 4
  %map11 = load [4 x [4 x i32]], ptr @15, align 4
  %x12 = load i32, ptr %x, align 4
  %13 = getelementptr inbounds [4 x i32], ptr @15, i32 %x12
  %14 = load [4 x i32], ptr %13, align 4
  %y13 = load i32, ptr %y, align 4
  %15 = getelementptr inbounds i32, ptr %13, i32 %y13
  %16 = load i32, ptr %15, align 4
  %17 = getelementptr inbounds i32, ptr @6, i32 %16
  %18 = load i32, ptr %17, align 4
  %19 = add nsw i32 %18, 1
  store i32 %19, ptr %i, align 4
  br label %for.cond14

for.cond14:                                       ; preds = %for.inc, %then
  %i15 = load i32, ptr %i, align 4
  %max_num_len = load i32, ptr @18, align 4
  %20 = icmp sle i32 %i15, %max_num_len
  %21 = sext i1 %20 to i32
  %22 = icmp ne i32 %21, 0
  br i1 %22, label %for.body16, label %for.last

for.body16:                                       ; preds = %for.cond14
  call void @putch(i32 95)
  %i17 = load i32, ptr %i, align 4
  %i18 = load i32, ptr %i, align 4
  %23 = add nsw i32 %i18, 1
  store i32 %23, ptr %i, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body16
  br label %for.cond14

for.last:                                         ; preds = %for.cond14
  %CHR_SPACE = load i32, ptr @13, align 4
  call void @putch(i32 %CHR_SPACE)
  br label %last

else:                                             ; preds = %for.body8
  %POW2 = load [20 x i32], ptr @5, align 4
  %map19 = load [4 x [4 x i32]], ptr @15, align 4
  %x20 = load i32, ptr %x, align 4
  %24 = getelementptr inbounds [4 x i32], ptr @15, i32 %x20
  %25 = load [4 x i32], ptr %24, align 4
  %y21 = load i32, ptr %y, align 4
  %26 = getelementptr inbounds i32, ptr %24, i32 %y21
  %27 = load i32, ptr %26, align 4
  %28 = getelementptr inbounds i32, ptr @5, i32 %27
  %29 = load i32, ptr %28, align 4
  call void @putint(i32 %29)
  %LEN_OF_POW223 = load [20 x i32], ptr @6, align 4
  %map24 = load [4 x [4 x i32]], ptr @15, align 4
  %x25 = load i32, ptr %x, align 4
  %30 = getelementptr inbounds [4 x i32], ptr @15, i32 %x25
  %31 = load [4 x i32], ptr %30, align 4
  %y26 = load i32, ptr %y, align 4
  %32 = getelementptr inbounds i32, ptr %30, i32 %y26
  %33 = load i32, ptr %32, align 4
  %34 = getelementptr inbounds i32, ptr @6, i32 %33
  %35 = load i32, ptr %34, align 4
  store i32 %35, ptr %i22, align 4
  br label %for.cond27

for.cond27:                                       ; preds = %for.inc34, %else
  %i28 = load i32, ptr %i22, align 4
  %max_num_len29 = load i32, ptr @18, align 4
  %36 = icmp sle i32 %i28, %max_num_len29
  %37 = sext i1 %36 to i32
  %38 = icmp ne i32 %37, 0
  br i1 %38, label %for.body30, label %for.last35

for.body30:                                       ; preds = %for.cond27
  %CHR_SPACE31 = load i32, ptr @13, align 4
  call void @putch(i32 %CHR_SPACE31)
  %i32 = load i32, ptr %i22, align 4
  %i33 = load i32, ptr %i22, align 4
  %39 = add nsw i32 %i33, 1
  store i32 %39, ptr %i22, align 4
  br label %for.inc34

for.inc34:                                        ; preds = %for.body30
  br label %for.cond27

for.last35:                                       ; preds = %for.cond27
  br label %last

last:                                             ; preds = %for.last35, %for.last
  %y36 = load i32, ptr %y, align 4
  %y37 = load i32, ptr %y, align 4
  %40 = add nsw i32 %y37, 1
  store i32 %40, ptr %y, align 4
  br label %for.inc38

for.inc38:                                        ; preds = %last
  br label %for.cond5

for.last39:                                       ; preds = %for.cond5
  %CHR_ENTER40 = load i32, ptr @14, align 4
  call void @putch(i32 %CHR_ENTER40)
  %x41 = load i32, ptr %x, align 4
  %x42 = load i32, ptr %x, align 4
  %41 = add nsw i32 %x42, 1
  store i32 %41, ptr %x, align 4
  br label %for.inc43

for.inc43:                                        ; preds = %for.last39
  br label %for.cond

for.last44:                                       ; preds = %for.cond
  ret void
}

define i32 @move_base(i32 %inc, ptr %var, ptr %other, ptr %x, ptr %y, i32 %is_dry_run) {
entry:
  %new_num_len = alloca i32, align 4
  %to_num = alloca i32, align 4
  %from_num = alloca i32, align 4
  %ptr_to = alloca i32, align 4
  %ptr_from = alloca i32, align 4
  %moved = alloca i32, align 4
  %end = alloca i32, align 4
  %start = alloca i32, align 4
  %inc1 = alloca i32, align 4
  store i32 %inc, ptr %inc1, align 4
  %var2 = alloca ptr, align 8
  store ptr %var, ptr %var2, align 8
  %other3 = alloca ptr, align 8
  store ptr %other, ptr %other3, align 8
  %x4 = alloca ptr, align 8
  store ptr %x, ptr %x4, align 8
  %y5 = alloca ptr, align 8
  store ptr %y, ptr %y5, align 8
  %is_dry_run6 = alloca i32, align 4
  store i32 %is_dry_run, ptr %is_dry_run6, align 4
  store i32 0, ptr %moved, align 4
  %inc7 = load i32, ptr %inc1, align 4
  %0 = icmp eq i32 %inc7, -1
  %1 = sext i1 %0 to i32
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %then, label %else

then:                                             ; preds = %entry
  %start8 = load i32, ptr %start, align 4
  %MAP_LEN = load i32, ptr @4, align 4
  %3 = sub nsw i32 %MAP_LEN, 1
  store i32 %3, ptr %start, align 4
  %end9 = load i32, ptr %end, align 4
  store i32 -1, ptr %end, align 4
  br label %last

else:                                             ; preds = %entry
  %start10 = load i32, ptr %start, align 4
  store i32 0, ptr %start, align 4
  %end11 = load i32, ptr %end, align 4
  %MAP_LEN12 = load i32, ptr @4, align 4
  store i32 %MAP_LEN12, ptr %end, align 4
  br label %last

last:                                             ; preds = %else, %then
  %other13 = load ptr, ptr %other3, align 8
  %4 = getelementptr inbounds i32, ptr %other13, i32 0
  %5 = load i32, ptr %4, align 4
  %start14 = load i32, ptr %start, align 4
  store i32 %start14, ptr %4, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc111, %last
  %other15 = load ptr, ptr %other3, align 8
  %6 = getelementptr inbounds i32, ptr %other15, i32 0
  %7 = load i32, ptr %6, align 4
  %end16 = load i32, ptr %end, align 4
  %8 = icmp ne i32 %7, %end16
  %9 = sext i1 %8 to i32
  %10 = icmp ne i32 %9, 0
  br i1 %10, label %for.body, label %for.last112

for.body:                                         ; preds = %for.cond
  %start17 = load i32, ptr %start, align 4
  %inc18 = load i32, ptr %inc1, align 4
  %11 = add nsw i32 %start17, %inc18
  store i32 %11, ptr %ptr_from, align 4
  %start19 = load i32, ptr %start, align 4
  store i32 %start19, ptr %ptr_to, align 4
  br label %for.cond20

for.cond20:                                       ; preds = %for.inc, %for.body
  %ptr_from21 = load i32, ptr %ptr_from, align 4
  %end22 = load i32, ptr %end, align 4
  %12 = icmp ne i32 %ptr_from21, %end22
  %13 = sext i1 %12 to i32
  %14 = icmp ne i32 %13, 0
  br i1 %14, label %for.body23, label %for.last

for.body23:                                       ; preds = %for.cond20
  %ptr_from24 = load i32, ptr %ptr_from, align 4
  %ptr_to25 = load i32, ptr %ptr_to, align 4
  %15 = icmp eq i32 %ptr_from24, %ptr_to25
  %16 = sext i1 %15 to i32
  %17 = icmp ne i32 %16, 0
  br i1 %17, label %then26, label %last30

then26:                                           ; preds = %for.body23
  %ptr_from27 = load i32, ptr %ptr_from, align 4
  %ptr_from28 = load i32, ptr %ptr_from, align 4
  %inc29 = load i32, ptr %inc1, align 4
  %18 = add nsw i32 %ptr_from28, %inc29
  store i32 %18, ptr %ptr_from, align 4
  br label %for.inc

last30:                                           ; preds = %for.body23
  %var31 = load ptr, ptr %var2, align 8
  %19 = getelementptr inbounds i32, ptr %var31, i32 0
  %20 = load i32, ptr %19, align 4
  %ptr_from32 = load i32, ptr %ptr_from, align 4
  store i32 %ptr_from32, ptr %19, align 4
  %map = load [4 x [4 x i32]], ptr @15, align 4
  %x33 = load ptr, ptr %x4, align 8
  %21 = getelementptr inbounds i32, ptr %x33, i32 0
  %22 = load i32, ptr %21, align 4
  %23 = getelementptr inbounds [4 x i32], ptr @15, i32 %22
  %24 = load [4 x i32], ptr %23, align 4
  %y34 = load ptr, ptr %y5, align 8
  %25 = getelementptr inbounds i32, ptr %y34, i32 0
  %26 = load i32, ptr %25, align 4
  %27 = getelementptr inbounds i32, ptr %23, i32 %26
  %28 = load i32, ptr %27, align 4
  store i32 %28, ptr %from_num, align 4
  %var35 = load ptr, ptr %var2, align 8
  %29 = getelementptr inbounds i32, ptr %var35, i32 0
  %30 = load i32, ptr %29, align 4
  %ptr_to36 = load i32, ptr %ptr_to, align 4
  store i32 %ptr_to36, ptr %29, align 4
  %map37 = load [4 x [4 x i32]], ptr @15, align 4
  %x38 = load ptr, ptr %x4, align 8
  %31 = getelementptr inbounds i32, ptr %x38, i32 0
  %32 = load i32, ptr %31, align 4
  %33 = getelementptr inbounds [4 x i32], ptr @15, i32 %32
  %34 = load [4 x i32], ptr %33, align 4
  %y39 = load ptr, ptr %y5, align 8
  %35 = getelementptr inbounds i32, ptr %y39, i32 0
  %36 = load i32, ptr %35, align 4
  %37 = getelementptr inbounds i32, ptr %33, i32 %36
  %38 = load i32, ptr %37, align 4
  store i32 %38, ptr %to_num, align 4
  %to_num40 = load i32, ptr %to_num, align 4
  %39 = icmp eq i32 %to_num40, 0
  %40 = sext i1 %39 to i32
  %41 = icmp ne i32 %40, 0
  br i1 %41, label %then41, label %else65

then41:                                           ; preds = %last30
  %from_num42 = load i32, ptr %from_num, align 4
  %42 = icmp eq i32 %from_num42, 0
  %43 = sext i1 %42 to i32
  %44 = icmp ne i32 %43, 0
  br i1 %44, label %then43, label %else47

then43:                                           ; preds = %then41
  %ptr_from44 = load i32, ptr %ptr_from, align 4
  %ptr_from45 = load i32, ptr %ptr_from, align 4
  %inc46 = load i32, ptr %inc1, align 4
  %45 = add nsw i32 %ptr_from45, %inc46
  store i32 %45, ptr %ptr_from, align 4
  br label %last64

else47:                                           ; preds = %then41
  %is_dry_run48 = load i32, ptr %is_dry_run6, align 4
  %46 = icmp ne i32 %is_dry_run48, 0
  br i1 %46, label %then49, label %last50

then49:                                           ; preds = %else47
  ret i32 1

last50:                                           ; preds = %else47
  %map51 = load [4 x [4 x i32]], ptr @15, align 4
  %x52 = load ptr, ptr %x4, align 8
  %47 = getelementptr inbounds i32, ptr %x52, i32 0
  %48 = load i32, ptr %47, align 4
  %49 = getelementptr inbounds [4 x i32], ptr @15, i32 %48
  %50 = load [4 x i32], ptr %49, align 4
  %y53 = load ptr, ptr %y5, align 8
  %51 = getelementptr inbounds i32, ptr %y53, i32 0
  %52 = load i32, ptr %51, align 4
  %53 = getelementptr inbounds i32, ptr %49, i32 %52
  %54 = load i32, ptr %53, align 4
  %from_num54 = load i32, ptr %from_num, align 4
  store i32 %from_num54, ptr %53, align 4
  %var55 = load ptr, ptr %var2, align 8
  %55 = getelementptr inbounds i32, ptr %var55, i32 0
  %56 = load i32, ptr %55, align 4
  %ptr_from56 = load i32, ptr %ptr_from, align 4
  store i32 %ptr_from56, ptr %55, align 4
  %map57 = load [4 x [4 x i32]], ptr @15, align 4
  %x58 = load ptr, ptr %x4, align 8
  %57 = getelementptr inbounds i32, ptr %x58, i32 0
  %58 = load i32, ptr %57, align 4
  %59 = getelementptr inbounds [4 x i32], ptr @15, i32 %58
  %60 = load [4 x i32], ptr %59, align 4
  %y59 = load ptr, ptr %y5, align 8
  %61 = getelementptr inbounds i32, ptr %y59, i32 0
  %62 = load i32, ptr %61, align 4
  %63 = getelementptr inbounds i32, ptr %59, i32 %62
  %64 = load i32, ptr %63, align 4
  store i32 0, ptr %63, align 4
  %moved60 = load i32, ptr %moved, align 4
  store i32 1, ptr %moved, align 4
  %ptr_from61 = load i32, ptr %ptr_from, align 4
  %ptr_from62 = load i32, ptr %ptr_from, align 4
  %inc63 = load i32, ptr %inc1, align 4
  %65 = add nsw i32 %ptr_from62, %inc63
  store i32 %65, ptr %ptr_from, align 4
  br label %last64

last64:                                           ; preds = %last50, %then43
  br label %last107

else65:                                           ; preds = %last30
  %from_num66 = load i32, ptr %from_num, align 4
  %to_num67 = load i32, ptr %to_num, align 4
  %66 = icmp eq i32 %from_num66, %to_num67
  %67 = sext i1 %66 to i32
  %68 = icmp ne i32 %67, 0
  br i1 %68, label %then68, label %else95

then68:                                           ; preds = %else65
  %is_dry_run69 = load i32, ptr %is_dry_run6, align 4
  %69 = icmp ne i32 %is_dry_run69, 0
  br i1 %69, label %then70, label %last71

then70:                                           ; preds = %then68
  ret i32 1

last71:                                           ; preds = %then68
  %var72 = load ptr, ptr %var2, align 8
  %70 = getelementptr inbounds i32, ptr %var72, i32 0
  %71 = load i32, ptr %70, align 4
  %ptr_to73 = load i32, ptr %ptr_to, align 4
  store i32 %ptr_to73, ptr %70, align 4
  %map74 = load [4 x [4 x i32]], ptr @15, align 4
  %x75 = load ptr, ptr %x4, align 8
  %72 = getelementptr inbounds i32, ptr %x75, i32 0
  %73 = load i32, ptr %72, align 4
  %74 = getelementptr inbounds [4 x i32], ptr @15, i32 %73
  %75 = load [4 x i32], ptr %74, align 4
  %y76 = load ptr, ptr %y5, align 8
  %76 = getelementptr inbounds i32, ptr %y76, i32 0
  %77 = load i32, ptr %76, align 4
  %78 = getelementptr inbounds i32, ptr %74, i32 %77
  %79 = load i32, ptr %78, align 4
  %to_num77 = load i32, ptr %to_num, align 4
  %80 = add nsw i32 %to_num77, 1
  store i32 %80, ptr %78, align 4
  %var78 = load ptr, ptr %var2, align 8
  %81 = getelementptr inbounds i32, ptr %var78, i32 0
  %82 = load i32, ptr %81, align 4
  %ptr_from79 = load i32, ptr %ptr_from, align 4
  store i32 %ptr_from79, ptr %81, align 4
  %map80 = load [4 x [4 x i32]], ptr @15, align 4
  %x81 = load ptr, ptr %x4, align 8
  %83 = getelementptr inbounds i32, ptr %x81, i32 0
  %84 = load i32, ptr %83, align 4
  %85 = getelementptr inbounds [4 x i32], ptr @15, i32 %84
  %86 = load [4 x i32], ptr %85, align 4
  %y82 = load ptr, ptr %y5, align 8
  %87 = getelementptr inbounds i32, ptr %y82, i32 0
  %88 = load i32, ptr %87, align 4
  %89 = getelementptr inbounds i32, ptr %85, i32 %88
  %90 = load i32, ptr %89, align 4
  store i32 0, ptr %89, align 4
  %LEN_OF_POW2 = load [20 x i32], ptr @6, align 4
  %to_num83 = load i32, ptr %to_num, align 4
  %91 = add nsw i32 %to_num83, 1
  %92 = getelementptr inbounds i32, ptr @6, i32 %91
  %93 = load i32, ptr %92, align 4
  store i32 %93, ptr %new_num_len, align 4
  %new_num_len84 = load i32, ptr %new_num_len, align 4
  %max_num_len = load i32, ptr @18, align 4
  %94 = icmp sgt i32 %new_num_len84, %max_num_len
  %95 = sext i1 %94 to i32
  %96 = icmp ne i32 %95, 0
  br i1 %96, label %then85, label %last88

then85:                                           ; preds = %last71
  %max_num_len86 = load i32, ptr @18, align 4
  %new_num_len87 = load i32, ptr %new_num_len, align 4
  store i32 %new_num_len87, ptr @18, align 4
  br label %last88

last88:                                           ; preds = %then85, %last71
  %score = load i32, ptr @16, align 4
  %score89 = load i32, ptr @16, align 4
  %POW2 = load [20 x i32], ptr @5, align 4
  %to_num90 = load i32, ptr %to_num, align 4
  %97 = add nsw i32 %to_num90, 1
  %98 = getelementptr inbounds i32, ptr @5, i32 %97
  %99 = load i32, ptr %98, align 4
  %100 = add nsw i32 %score89, %99
  store i32 %100, ptr @16, align 4
  %moved91 = load i32, ptr %moved, align 4
  store i32 1, ptr %moved, align 4
  %ptr_to92 = load i32, ptr %ptr_to, align 4
  %ptr_to93 = load i32, ptr %ptr_to, align 4
  %inc94 = load i32, ptr %inc1, align 4
  %101 = add nsw i32 %ptr_to93, %inc94
  store i32 %101, ptr %ptr_to, align 4
  br label %last106

else95:                                           ; preds = %else65
  %from_num96 = load i32, ptr %from_num, align 4
  %102 = icmp eq i32 %from_num96, 0
  %103 = sext i1 %102 to i32
  %104 = icmp ne i32 %103, 0
  br i1 %104, label %then97, label %else101

then97:                                           ; preds = %else95
  %ptr_from98 = load i32, ptr %ptr_from, align 4
  %ptr_from99 = load i32, ptr %ptr_from, align 4
  %inc100 = load i32, ptr %inc1, align 4
  %105 = add nsw i32 %ptr_from99, %inc100
  store i32 %105, ptr %ptr_from, align 4
  br label %last105

else101:                                          ; preds = %else95
  %ptr_to102 = load i32, ptr %ptr_to, align 4
  %ptr_to103 = load i32, ptr %ptr_to, align 4
  %inc104 = load i32, ptr %inc1, align 4
  %106 = add nsw i32 %ptr_to103, %inc104
  store i32 %106, ptr %ptr_to, align 4
  br label %last105

last105:                                          ; preds = %else101, %then97
  br label %last106

last106:                                          ; preds = %last105, %last88
  br label %last107

last107:                                          ; preds = %last106, %last64
  br label %for.inc

for.inc:                                          ; preds = %last107, %then26
  br label %for.cond20

for.last:                                         ; preds = %for.cond20
  %other108 = load ptr, ptr %other3, align 8
  %107 = getelementptr inbounds i32, ptr %other108, i32 0
  %108 = load i32, ptr %107, align 4
  %other109 = load ptr, ptr %other3, align 8
  %109 = getelementptr inbounds i32, ptr %other109, i32 0
  %110 = load i32, ptr %109, align 4
  %inc110 = load i32, ptr %inc1, align 4
  %111 = add nsw i32 %110, %inc110
  store i32 %111, ptr %107, align 4
  br label %for.inc111

for.inc111:                                       ; preds = %for.last
  br label %for.cond

for.last112:                                      ; preds = %for.cond
  %moved113 = load i32, ptr %moved, align 4
  ret i32 %moved113
}

define void @generate() {
entry:
  %num = alloca i32, align 4
  %empty = alloca i32, align 4
  %chosen_y = alloca i32, align 4
  %chosen_x = alloca i32, align 4
  %y = alloca i32, align 4
  %x = alloca i32, align 4
  store i32 0, ptr %x, align 4
  store i32 0, ptr %empty, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc22, %entry
  %x1 = load i32, ptr %x, align 4
  %MAP_LEN = load i32, ptr @4, align 4
  %0 = icmp slt i32 %x1, %MAP_LEN
  %1 = sext i1 %0 to i32
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %for.body, label %for.last23

for.body:                                         ; preds = %for.cond
  %y2 = load i32, ptr %y, align 4
  store i32 0, ptr %y, align 4
  br label %for.cond3

for.cond3:                                        ; preds = %for.inc, %for.body
  %y4 = load i32, ptr %y, align 4
  %MAP_LEN5 = load i32, ptr @4, align 4
  %3 = icmp slt i32 %y4, %MAP_LEN5
  %4 = sext i1 %3 to i32
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %for.body6, label %for.last

for.body6:                                        ; preds = %for.cond3
  %map = load [4 x [4 x i32]], ptr @15, align 4
  %x7 = load i32, ptr %x, align 4
  %6 = getelementptr inbounds [4 x i32], ptr @15, i32 %x7
  %7 = load [4 x i32], ptr %6, align 4
  %y8 = load i32, ptr %y, align 4
  %8 = getelementptr inbounds i32, ptr %6, i32 %y8
  %9 = load i32, ptr %8, align 4
  %10 = icmp eq i32 %9, 0
  %11 = sext i1 %10 to i32
  %12 = icmp ne i32 %11, 0
  br i1 %12, label %then, label %last17

then:                                             ; preds = %for.body6
  %empty9 = load i32, ptr %empty, align 4
  %empty10 = load i32, ptr %empty, align 4
  %13 = add nsw i32 %empty10, 1
  store i32 %13, ptr %empty, align 4
  %14 = call i32 @rand()
  %empty11 = load i32, ptr %empty, align 4
  %15 = srem i32 %14, %empty11
  %16 = icmp eq i32 %15, 0
  %17 = sext i1 %16 to i32
  %18 = icmp ne i32 %17, 0
  br i1 %18, label %then12, label %last

then12:                                           ; preds = %then
  %chosen_x13 = load i32, ptr %chosen_x, align 4
  %x14 = load i32, ptr %x, align 4
  store i32 %x14, ptr %chosen_x, align 4
  %chosen_y15 = load i32, ptr %chosen_y, align 4
  %y16 = load i32, ptr %y, align 4
  store i32 %y16, ptr %chosen_y, align 4
  br label %last

last:                                             ; preds = %then12, %then
  br label %last17

last17:                                           ; preds = %last, %for.body6
  %y18 = load i32, ptr %y, align 4
  %y19 = load i32, ptr %y, align 4
  %19 = add nsw i32 %y19, 1
  store i32 %19, ptr %y, align 4
  br label %for.inc

for.inc:                                          ; preds = %last17
  br label %for.cond3

for.last:                                         ; preds = %for.cond3
  %x20 = load i32, ptr %x, align 4
  %x21 = load i32, ptr %x, align 4
  %20 = add nsw i32 %x21, 1
  store i32 %20, ptr %x, align 4
  br label %for.inc22

for.inc22:                                        ; preds = %for.last
  br label %for.cond

for.last23:                                       ; preds = %for.cond
  %21 = call i32 @rand()
  %22 = srem i32 %21, 2
  %23 = icmp slt i32 %22, 1
  %24 = sext i1 %23 to i32
  %25 = icmp ne i32 %24, 0
  br i1 %25, label %then24, label %else

then24:                                           ; preds = %for.last23
  %num25 = load i32, ptr %num, align 4
  store i32 1, ptr %num, align 4
  br label %last27

else:                                             ; preds = %for.last23
  %num26 = load i32, ptr %num, align 4
  store i32 2, ptr %num, align 4
  br label %last27

last27:                                           ; preds = %else, %then24
  %map28 = load [4 x [4 x i32]], ptr @15, align 4
  %chosen_x29 = load i32, ptr %chosen_x, align 4
  %26 = getelementptr inbounds [4 x i32], ptr @15, i32 %chosen_x29
  %27 = load [4 x i32], ptr %26, align 4
  %chosen_y30 = load i32, ptr %chosen_y, align 4
  %28 = getelementptr inbounds i32, ptr %26, i32 %chosen_y30
  %29 = load i32, ptr %28, align 4
  %num31 = load i32, ptr %num, align 4
  store i32 %num31, ptr %28, align 4
  ret void
}

define void @move(i32 %pos) {
entry:
  %moved = alloca i32, align 4
  %inc = alloca i32, align 4
  %y = alloca [1 x i32], align 4
  %x = alloca [1 x i32], align 4
  %pos1 = alloca i32, align 4
  store i32 %pos, ptr %pos1, align 4
  %pos2 = load i32, ptr %pos1, align 4
  %0 = srem i32 %pos2, 2
  %1 = mul nsw i32 %0, 2
  %2 = sub nsw i32 1, %1
  store i32 %2, ptr %inc, align 4
  %pos3 = load i32, ptr %pos1, align 4
  %3 = sdiv i32 %pos3, 2
  %4 = icmp eq i32 %3, 0
  %5 = sext i1 %4 to i32
  %6 = icmp ne i32 %5, 0
  br i1 %6, label %then, label %else

then:                                             ; preds = %entry
  %moved4 = load i32, ptr %moved, align 4
  %inc5 = load i32, ptr %inc, align 4
  %x6 = load [1 x i32], ptr %x, align 4
  %y7 = load [1 x i32], ptr %y, align 4
  %x8 = load [1 x i32], ptr %x, align 4
  %y9 = load [1 x i32], ptr %y, align 4
  %7 = call i32 @move_base(i32 %inc5, ptr %x, ptr %y, ptr %x, ptr %y, i32 0)
  store i32 %7, ptr %moved, align 4
  br label %last

else:                                             ; preds = %entry
  %moved10 = load i32, ptr %moved, align 4
  %inc11 = load i32, ptr %inc, align 4
  %y12 = load [1 x i32], ptr %y, align 4
  %x13 = load [1 x i32], ptr %x, align 4
  %x14 = load [1 x i32], ptr %x, align 4
  %y15 = load [1 x i32], ptr %y, align 4
  %8 = call i32 @move_base(i32 %inc11, ptr %y, ptr %x, ptr %x, ptr %y, i32 0)
  store i32 %8, ptr %moved, align 4
  br label %last

last:                                             ; preds = %else, %then
  %moved16 = load i32, ptr %moved, align 4
  %9 = icmp ne i32 %moved16, 0
  %10 = xor i1 %9, true
  %11 = zext i1 %10 to i32
  %12 = icmp ne i32 %11, 0
  br i1 %12, label %then17, label %last18

then17:                                           ; preds = %last
  %STR_INVALID = load [26 x i32], ptr @12, align 4
  call void @put_string(ptr @12)
  %CHR_ENTER = load i32, ptr @14, align 4
  call void @putch(i32 %CHR_ENTER)
  ret void

last18:                                           ; preds = %last
  %step = load i32, ptr @17, align 4
  %step19 = load i32, ptr @17, align 4
  %13 = add nsw i32 %step19, 1
  store i32 %13, ptr @17, align 4
  call void @generate()
  call void @print_map()
  ret void
}

define i32 @try_move() {
entry:
  %y = alloca [1 x i32], align 4
  %x = alloca [1 x i32], align 4
  %x1 = load [1 x i32], ptr %x, align 4
  %y2 = load [1 x i32], ptr %y, align 4
  %x3 = load [1 x i32], ptr %x, align 4
  %y4 = load [1 x i32], ptr %y, align 4
  %0 = call i32 @move_base(i32 1, ptr %x, ptr %y, ptr %x, ptr %y, i32 1)
  %1 = icmp ne i32 %0, 0
  br i1 %1, label %trueBB, label %nextBB

nextBB:                                           ; preds = %entry
  %y5 = load [1 x i32], ptr %y, align 4
  %x6 = load [1 x i32], ptr %x, align 4
  %x7 = load [1 x i32], ptr %x, align 4
  %y8 = load [1 x i32], ptr %y, align 4
  %2 = call i32 @move_base(i32 1, ptr %y, ptr %x, ptr %x, ptr %y, i32 1)
  %3 = icmp ne i32 %2, 0
  %4 = zext i1 %3 to i32
  br label %mergeBB

trueBB:                                           ; preds = %entry
  br label %mergeBB

mergeBB:                                          ; preds = %trueBB, %nextBB
  %5 = phi i32 [ %4, %nextBB ], [ 1, %trueBB ]
  %6 = icmp ne i32 %5, 0
  br i1 %6, label %trueBB14, label %nextBB9

nextBB9:                                          ; preds = %mergeBB
  %x10 = load [1 x i32], ptr %x, align 4
  %y11 = load [1 x i32], ptr %y, align 4
  %x12 = load [1 x i32], ptr %x, align 4
  %y13 = load [1 x i32], ptr %y, align 4
  %7 = call i32 @move_base(i32 -1, ptr %x, ptr %y, ptr %x, ptr %y, i32 1)
  %8 = icmp ne i32 %7, 0
  %9 = zext i1 %8 to i32
  br label %mergeBB15

trueBB14:                                         ; preds = %mergeBB
  br label %mergeBB15

mergeBB15:                                        ; preds = %trueBB14, %nextBB9
  %10 = phi i32 [ %9, %nextBB9 ], [ 1, %trueBB14 ]
  %11 = icmp ne i32 %10, 0
  br i1 %11, label %trueBB21, label %nextBB16

nextBB16:                                         ; preds = %mergeBB15
  %y17 = load [1 x i32], ptr %y, align 4
  %x18 = load [1 x i32], ptr %x, align 4
  %x19 = load [1 x i32], ptr %x, align 4
  %y20 = load [1 x i32], ptr %y, align 4
  %12 = call i32 @move_base(i32 -1, ptr %y, ptr %x, ptr %x, ptr %y, i32 1)
  %13 = icmp ne i32 %12, 0
  %14 = zext i1 %13 to i32
  br label %mergeBB22

trueBB21:                                         ; preds = %mergeBB15
  br label %mergeBB22

mergeBB22:                                        ; preds = %trueBB21, %nextBB16
  %15 = phi i32 [ %14, %nextBB16 ], [ 1, %trueBB21 ]
  ret i32 %15
}

define i32 @main() {
entry:
  %ch = alloca i32, align 4
  %STR_INIT = load [25 x i32], ptr @7, align 4
  call void @put_string(ptr @7)
  %seed = load i32, ptr @20, align 4
  %0 = call i32 @getint()
  store i32 %0, ptr @20, align 4
  call void @init()
  call void @generate()
  call void @print_map()
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %alive = load i32, ptr @19, align 4
  %1 = icmp ne i32 %alive, 0
  br i1 %1, label %for.body, label %for.last

for.body:                                         ; preds = %for.cond
  %2 = call i32 @getch()
  store i32 %2, ptr %ch, align 4
  %ch1 = load i32, ptr %ch, align 4
  %3 = icmp eq i32 %ch1, 119
  %4 = sext i1 %3 to i32
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %then, label %else

then:                                             ; preds = %for.body
  %UP = load i32, ptr @0, align 4
  call void @move(i32 %UP)
  br label %last42

else:                                             ; preds = %for.body
  %ch2 = load i32, ptr %ch, align 4
  %6 = icmp eq i32 %ch2, 97
  %7 = sext i1 %6 to i32
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %then3, label %else4

then3:                                            ; preds = %else
  %LEFT = load i32, ptr @2, align 4
  call void @move(i32 %LEFT)
  br label %last41

else4:                                            ; preds = %else
  %ch5 = load i32, ptr %ch, align 4
  %9 = icmp eq i32 %ch5, 115
  %10 = sext i1 %9 to i32
  %11 = icmp ne i32 %10, 0
  br i1 %11, label %then6, label %else7

then6:                                            ; preds = %else4
  %DOWN = load i32, ptr @1, align 4
  call void @move(i32 %DOWN)
  br label %last40

else7:                                            ; preds = %else4
  %ch8 = load i32, ptr %ch, align 4
  %12 = icmp eq i32 %ch8, 100
  %13 = sext i1 %12 to i32
  %14 = icmp ne i32 %13, 0
  br i1 %14, label %then9, label %else10

then9:                                            ; preds = %else7
  %RIGHT = load i32, ptr @3, align 4
  call void @move(i32 %RIGHT)
  br label %last39

else10:                                           ; preds = %else7
  %ch11 = load i32, ptr %ch, align 4
  %15 = icmp eq i32 %ch11, 104
  %16 = sext i1 %15 to i32
  %17 = icmp ne i32 %16, 0
  br i1 %17, label %then12, label %else13

then12:                                           ; preds = %else10
  %STR_HELP = load [62 x i32], ptr @8, align 4
  call void @put_string(ptr @8)
  br label %last38

else13:                                           ; preds = %else10
  %ch14 = load i32, ptr %ch, align 4
  %18 = icmp eq i32 %ch14, 113
  %19 = sext i1 %18 to i32
  %20 = icmp ne i32 %19, 0
  br i1 %20, label %trueBB, label %nextBB

nextBB:                                           ; preds = %else13
  %ch15 = load i32, ptr %ch, align 4
  %21 = icmp eq i32 %ch15, -1
  %22 = sext i1 %21 to i32
  %23 = icmp ne i32 %22, 0
  %24 = zext i1 %23 to i32
  br label %mergeBB

trueBB:                                           ; preds = %else13
  br label %mergeBB

mergeBB:                                          ; preds = %trueBB, %nextBB
  %25 = phi i32 [ %24, %nextBB ], [ 1, %trueBB ]
  %26 = icmp ne i32 %25, 0
  br i1 %26, label %then16, label %else17

then16:                                           ; preds = %mergeBB
  %STR_GG = load [12 x i32], ptr @11, align 4
  call void @put_string(ptr @11)
  ret i32 0

else17:                                           ; preds = %mergeBB
  %ch18 = load i32, ptr %ch, align 4
  %27 = icmp eq i32 %ch18, 112
  %28 = sext i1 %27 to i32
  %29 = icmp ne i32 %28, 0
  br i1 %29, label %then19, label %else20

then19:                                           ; preds = %else17
  %CHR_ENTER = load i32, ptr @14, align 4
  call void @putch(i32 %CHR_ENTER)
  call void @print_map()
  br label %last36

else20:                                           ; preds = %else17
  %ch21 = load i32, ptr %ch, align 4
  %30 = icmp eq i32 %ch21, 32
  %31 = sext i1 %30 to i32
  %32 = icmp ne i32 %31, 0
  br i1 %32, label %trueBB24, label %nextBB22

nextBB22:                                         ; preds = %else20
  %ch23 = load i32, ptr %ch, align 4
  %33 = icmp eq i32 %ch23, 10
  %34 = sext i1 %33 to i32
  %35 = icmp ne i32 %34, 0
  %36 = zext i1 %35 to i32
  br label %mergeBB25

trueBB24:                                         ; preds = %else20
  br label %mergeBB25

mergeBB25:                                        ; preds = %trueBB24, %nextBB22
  %37 = phi i32 [ %36, %nextBB22 ], [ 1, %trueBB24 ]
  %38 = icmp ne i32 %37, 0
  br i1 %38, label %trueBB28, label %nextBB26

nextBB26:                                         ; preds = %mergeBB25
  %ch27 = load i32, ptr %ch, align 4
  %39 = icmp eq i32 %ch27, 13
  %40 = sext i1 %39 to i32
  %41 = icmp ne i32 %40, 0
  %42 = zext i1 %41 to i32
  br label %mergeBB29

trueBB28:                                         ; preds = %mergeBB25
  br label %mergeBB29

mergeBB29:                                        ; preds = %trueBB28, %nextBB26
  %43 = phi i32 [ %42, %nextBB26 ], [ 1, %trueBB28 ]
  %44 = icmp ne i32 %43, 0
  br i1 %44, label %then30, label %else31

then30:                                           ; preds = %mergeBB29
  br label %for.inc

else31:                                           ; preds = %mergeBB29
  %STR_INVALID = load [26 x i32], ptr @12, align 4
  call void @put_string(ptr @12)
  %CHR_ENTER32 = load i32, ptr @14, align 4
  call void @putch(i32 %CHR_ENTER32)
  %seed33 = load i32, ptr @20, align 4
  %seed34 = load i32, ptr @20, align 4
  %ch35 = load i32, ptr %ch, align 4
  %45 = add nsw i32 %seed34, %ch35
  %46 = srem i32 %45, 1073741824
  store i32 %46, ptr @20, align 4
  br label %last

last:                                             ; preds = %else31
  br label %last36

last36:                                           ; preds = %last, %then19
  br label %last37

last37:                                           ; preds = %last36
  br label %last38

last38:                                           ; preds = %last37, %then12
  br label %last39

last39:                                           ; preds = %last38, %then9
  br label %last40

last40:                                           ; preds = %last39, %then6
  br label %last41

last41:                                           ; preds = %last40, %then3
  br label %last42

last42:                                           ; preds = %last41, %then
  %47 = call i32 @try_move()
  %48 = icmp ne i32 %47, 0
  %49 = xor i1 %48, true
  %50 = zext i1 %49 to i32
  %51 = icmp ne i32 %50, 0
  br i1 %51, label %then43, label %last45

then43:                                           ; preds = %last42
  %STR_GG44 = load [12 x i32], ptr @11, align 4
  call void @put_string(ptr @11)
  br label %for.last

last45:                                           ; preds = %last42
  br label %for.inc

for.inc:                                          ; preds = %last45, %then30
  br label %for.cond

for.last:                                         ; preds = %then43, %for.cond
  ret i32 0
}
