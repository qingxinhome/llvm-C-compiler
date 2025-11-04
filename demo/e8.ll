; ModuleID = 'demo/e8.c'
source_filename = "demo/e8.c"

%A = type { ptr }

@0 = global %A zeroinitializer, align 8

define i32 @main() {
entry:
  ret i32 0
}
