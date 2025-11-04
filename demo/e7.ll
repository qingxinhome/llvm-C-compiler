; ModuleID = 'demo/e7.c'
source_filename = "demo/e7.c"

%Point = type { ptr }

define i32 @getListCount(ptr %first) {
entry:
  %i = alloca i32, align 4
  %current = alloca ptr, align 8
  %first1 = alloca ptr, align 8
  store ptr %first, ptr %first1, align 8
  %first2 = load ptr, ptr %first1, align 8
  store ptr %first2, ptr %current, align 8
  store i32 0, ptr %i, align 4
  br label %for.init

for.init:                                         ; preds = %entry
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %for.init
  %current3 = load ptr, ptr %current, align 8
  %0 = ptrtoint ptr %current3 to i32
  %1 = icmp ne i32 %0, 0
  br i1 %1, label %for.body, label %for.last

for.body:                                         ; preds = %for.cond
  %current4 = load ptr, ptr %current, align 8
  %current5 = load ptr, ptr %current, align 8
  %2 = getelementptr inbounds %Point, ptr %current5, i32 0, i32 0
  %3 = load ptr, ptr %2, align 8
  store ptr %3, ptr %current, align 8
  %i6 = load i32, ptr %i, align 4
  %4 = add i32 %i6, 1
  store i32 %4, ptr %i, align 4
  br label %for.inc

for.inc:                                          ; preds = %for.body
  br label %for.cond

for.last:                                         ; preds = %for.cond
  %i7 = load i32, ptr %i, align 4
  ret i32 %i7
}

define i32 @main() {
entry:
  %third = alloca %Point, align 8
  %second = alloca %Point, align 8
  %first = alloca %Point, align 8
  store ptr null, ptr %first, align 8
  store ptr null, ptr %second, align 8
  store ptr null, ptr %third, align 8
  %first1 = load %Point, ptr %first, align 8
  %0 = getelementptr inbounds %Point, ptr %first, i32 0, i32 0
  %1 = load ptr, ptr %0, align 8
  %second2 = load %Point, ptr %second, align 8
  store ptr %second, ptr %0, align 8
  %second3 = load %Point, ptr %second, align 8
  %2 = getelementptr inbounds %Point, ptr %second, i32 0, i32 0
  %3 = load ptr, ptr %2, align 8
  %third4 = load %Point, ptr %third, align 8
  store ptr %third, ptr %2, align 8
  %third5 = load %Point, ptr %third, align 8
  %4 = getelementptr inbounds %Point, ptr %third, i32 0, i32 0
  %5 = load ptr, ptr %4, align 8
  store i32 0, ptr %4, align 4
  %first6 = load %Point, ptr %first, align 8
  %6 = call i32 @getListCount(ptr %first)
  ret i32 %6
}
