#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int foo() {
    printf("called foo()\n");

    return 2;
}

int print(char *literal) {
    printf("%s\n", literal);
}
int printNum(int num) {
    printf("%d\n", num);
}

int bar(int x, int y) {
    int result = x + y;
    printf("%d + %d = %d\n", x, y, result);

    return result;
}

int add6(int a, int b, int c, int d, int e, int f) {
    return a + b + c + d + e + f;
}

void alloc4(int **p, int a, int b, int c, int d) {
    int *tmp = malloc(4 * 4);
    tmp[0] = a;
    tmp[1] = b;
    tmp[2] = c;
    tmp[3] = d;

    *p = tmp;

    return;
}
