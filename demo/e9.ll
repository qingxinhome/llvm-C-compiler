; ModuleID = 'demo/e9.c'
source_filename = "demo/e9.c"

%A = type { i32, i32, ptr }

define %A @getNewStruct(%A %a) {
entry:
  %newA = alloca %A, align 8
  %a1 = alloca %A, align 8
  store %A %a, ptr %a1, align 8
  %newA2 = load %A, ptr %newA, align 8
  %0 = getelementptr inbounds %A, ptr %newA, i32 0, i32 0
  %1 = load i32, ptr %0, align 4
  %a3 = load %A, ptr %a1, align 8
  %2 = getelementptr inbounds %A, ptr %a1, i32 0, i32 0
  %3 = load i32, ptr %2, align 4
  %4 = add nsw i32 %3, 102
  store i32 %4, ptr %0, align 4
  %newA4 = load %A, ptr %newA, align 8
  %5 = getelementptr inbounds %A, ptr %newA, i32 0, i32 1
  %6 = load i32, ptr %5, align 4
  %a5 = load %A, ptr %a1, align 8
  %7 = getelementptr inbounds %A, ptr %a1, i32 0, i32 1
  %8 = load i32, ptr %7, align 4
  %9 = add nsw i32 %8, 1024
  store i32 %9, ptr %5, align 4
  %newA6 = load %A, ptr %newA, align 8
  %10 = getelementptr inbounds %A, ptr %newA, i32 0, i32 2
  %11 = load ptr, ptr %10, align 8
  store i32 0, ptr %10, align 4
  %newA7 = load %A, ptr %newA, align 8
  ret %A %newA7
}

define i32 @main() {
entry:
  %newA = alloca %A, align 8
  %a = alloca %A, align 8
  %0 = getelementptr inbounds %A, ptr %a, i32 0, i32 0
  store i32 1, ptr %0, align 4
  %1 = getelementptr inbounds %A, ptr %a, i32 0, i32 1
  store i32 2, ptr %1, align 4
  %2 = getelementptr inbounds %A, ptr %a, i32 0, i32 2
  store ptr null, ptr %2, align 8
  %a1 = load %A, ptr %a, align 8
  %3 = call %A @getNewStruct(%A %a1)
  store %A %3, ptr %newA, align 8
  %newA2 = load %A, ptr %newA, align 8
  %4 = getelementptr inbounds %A, ptr %newA, i32 0, i32 0
  %5 = load i32, ptr %4, align 4
  %newA3 = load %A, ptr %newA, align 8
  %6 = getelementptr inbounds %A, ptr %newA, i32 0, i32 1
  %7 = load i32, ptr %6, align 4
  %8 = add nsw i32 %5, %7
  ret i32 %8
}
