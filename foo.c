#include <stdio.h>
#include <stdlib.h>

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

void alloc4(int *p, int a, int b, int c, int d) {
    p = malloc(4 * 4);
    p[0] = a;
    p[1] = b;
    p[2] = c;
    p[3] = d;
    return;
}
