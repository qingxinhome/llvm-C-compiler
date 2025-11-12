int printf(char * fmg, ...);int main(){struct student{[20]char name;int age;score;} stu = 119,105,116,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,20,99;struct student{[20]char name;int age;score;} *p1 = &stu;struct student{[20]char name;int age;score;} *p2 = &stu;printf(name: %s\n,p1->name);printf(name: %s\n,p2->name);return 0;}; ModuleID = 'demo/e20.c'
source_filename = "demo/e20.c"

%student = type { [20 x i8], i32, float }

@"name: %s\0A" = private constant [10 x i8] c"name: %s\0A\00"
@"name: %s\0A.1" = private constant [10 x i8] c"name: %s\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %p2 = alloca ptr, align 8
  %p1 = alloca ptr, align 8
  %stu = alloca %student, align 4
  %0 = getelementptr inbounds %student, ptr %stu, i32 0, i32 0, i32 0
  store i8 119, ptr %0, align 1
  %1 = getelementptr inbounds %student, ptr %stu, i32 0, i32 0, i32 1
  store i8 105, ptr %1, align 1
  %2 = getelementptr inbounds %student, ptr %stu, i32 0, i32 0, i32 2
  store i8 116, ptr %2, align 1
  %3 = getelementptr inbounds %student, ptr %stu, i32 0, i32 0, i32 3
  store i8 0, ptr %3, align 1
  %4 = getelementptr inbounds %student, ptr %stu, i32 0, i32 0, i32 4
  store i8 0, ptr %4, align 1
  %5 = getelementptr inbounds %student, ptr %stu, i32 0, i32 0, i32 5
  store i8 0, ptr %5, align 1
  %6 = getelementptr inbounds %student, ptr %stu, i32 0, i32 0, i32 6
  store i8 0, ptr %6, align 1
  %7 = getelementptr inbounds %student, ptr %stu, i32 0, i32 0, i32 7
  store i8 0, ptr %7, align 1
  %8 = getelementptr inbounds %student, ptr %stu, i32 0, i32 0, i32 8
  store i8 0, ptr %8, align 1
  %9 = getelementptr inbounds %student, ptr %stu, i32 0, i32 0, i32 9
  store i8 0, ptr %9, align 1
  %10 = getelementptr inbounds %student, ptr %stu, i32 0, i32 0, i32 10
  store i8 0, ptr %10, align 1
  %11 = getelementptr inbounds %student, ptr %stu, i32 0, i32 0, i32 11
  store i8 0, ptr %11, align 1
  %12 = getelementptr inbounds %student, ptr %stu, i32 0, i32 0, i32 12
  store i8 0, ptr %12, align 1
  %13 = getelementptr inbounds %student, ptr %stu, i32 0, i32 0, i32 13
  store i8 0, ptr %13, align 1
  %14 = getelementptr inbounds %student, ptr %stu, i32 0, i32 0, i32 14
  store i8 0, ptr %14, align 1
  %15 = getelementptr inbounds %student, ptr %stu, i32 0, i32 0, i32 15
  store i8 0, ptr %15, align 1
  %16 = getelementptr inbounds %student, ptr %stu, i32 0, i32 0, i32 16
  store i8 0, ptr %16, align 1
  %17 = getelementptr inbounds %student, ptr %stu, i32 0, i32 0, i32 17
  store i8 0, ptr %17, align 1
  %18 = getelementptr inbounds %student, ptr %stu, i32 0, i32 0, i32 18
  store i8 0, ptr %18, align 1
  %19 = getelementptr inbounds %student, ptr %stu, i32 0, i32 0, i32 19
  store i8 0, ptr %19, align 1
  %20 = getelementptr inbounds %student, ptr %stu, i32 0, i32 1
  store i32 20, ptr %20, align 4
  %21 = getelementptr inbounds %student, ptr %stu, i32 0, i32 2
  store float 9.900000e+01, ptr %21, align 4
  %stu1 = load %student, ptr %stu, align 4
  store ptr %stu, ptr %p1, align 8
  %stu2 = load %student, ptr %stu, align 4
  store ptr %stu, ptr %p2, align 8
  %p13 = load ptr, ptr %p1, align 8
  %22 = getelementptr inbounds %student, ptr %p13, i32 0, i32 0
  %23 = load [20 x i8], ptr %22, align 1
  %24 = call i32 (ptr, ...) @printf(ptr @"name: %s\0A", ptr %22)
  %p24 = load ptr, ptr %p2, align 8
  %25 = getelementptr inbounds %student, ptr %p24, i32 0, i32 0
  %26 = load [20 x i8], ptr %25, align 1
  %27 = call i32 (ptr, ...) @printf(ptr @"name: %s\0A.1", ptr %25)
  ret i32 0
}
name: wit
name: wit
