int printf(char * fmg, ...);[10]int arrayint main(){array[9] = 100;printf(array[9] = %d\n,array[9]);return 0;}; ModuleID = 'demo/e21.c'
source_filename = "demo/e21.c"

@0 = global [10 x i32] zeroinitializer, align 4
@"array[9] = %d\0A" = private constant [15 x i8] c"array[9] = %d\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %array = load [10 x i32], ptr @0, align 4
  %0 = load i32, ptr getelementptr inbounds (i32, ptr @0, i32 9), align 4
  store i32 100, ptr getelementptr inbounds (i32, ptr @0, i32 9), align 4
  %array1 = load [10 x i32], ptr @0, align 4
  %1 = load i32, ptr getelementptr inbounds (i32, ptr @0, i32 9), align 4
  %2 = call i32 (ptr, ...) @printf(ptr @"array[9] = %d\0A", i32 %1)
  ret i32 0
}
array[9] = 100
