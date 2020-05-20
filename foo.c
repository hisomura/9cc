#include <stdio.h>

int foo() {
    printf("called foo()\n");

    return 2;
}

int bar(int x, int y) {
    int result = x + y;
    printf("%d + %d = %d\n", x, y, result);

    return result;
}

int add6(int a, int b, int c, int d, int e, int f) {
    return a + b + c + d + e + f;
}