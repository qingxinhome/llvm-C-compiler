int printf(const char *fmg, ...);

int main() {
    int a = 0;  // 假设用户很懒
    int b = 1;  // 但完成了一次作业

    if (a > 0 && b > 0) {               // 逻辑或：只要有一个为真就通过
        printf("通过了！因为 b > 0\n");   // 只打印这一行
    } else {
        printf("没通过\n");
    }

    // 短路演示：如果 a>0 为真，右边的 printf 根本不会执行！
    if (a > 0 && printf("我被执行了！\n")) {
        printf("a>0 时这里不会打印第二句\n");
    }

    return 0;
}