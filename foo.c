#include <stdio.h>

int foo() {
    printf("OK\n");

    return 2;
}

int bar(int x, int y) {
    int result = x + y;
    printf("%d\n", result);

    return result;
}

int add6(int a, int b, int c, int d, int e, int f) {
    return a + b + c + d + e + f;
}