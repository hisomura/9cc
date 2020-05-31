/**
 * 第三引数を埋めるスクリプト
 * $ perl -i -pe 's{assert\((.*?), (.*), ".*"\);}{($a,$b)=($1,$2); (($c=$2) =~ s/([\\"])/\\\1/g); "assert($a, $b, \"$c\");"}ge' tests/tests.c
 */

int sub_char(char a, char b, char c) { return a-b-c; } // FIXME 下方にもっていくとエラーになる
int test_block() { int x=2; { int x=3; } { int y=4; return x; }} // FIXME 下方にもっていくとエラーになる
int test(int input) { return input; }
int sub(int x, int y){ return x - y; }
int fib(int x){ if (x<=1) return 1; return fib(x-1) + fib(x-2); }
int a;
int *b;
int *c[8][4];
int x;
int y;
int arr1[4];

int return_test() { ({ 0; return 1; 2; }); return 3; }
int deref(int *num_to) { return *num_to; }
int array_access(int *arr, int index) { return *(arr+index); }
int calc(int max, int *ad) {int i;for(i=0;i<max;i=i+1){*ad=*ad+i;}}

int main() {
    assert(21, 5 + 20 - 4, "5 + 20 - 4");
    assert(8, 2 * 4, "2 * 4");
    assert(1, 4 / 3, "4 / 3");
    assert(6, (4 + 2), "(4 + 2)");
    assert(1, (21 + 3) / 8 / 3, "(21 + 3) / 8 / 3");
    assert(10, -10 + 20, "-10 + 20");
    assert(30, 10 - -20, "10 - -20");
    assert(1, -3 * +5 + 16, "-3 * +5 + 16");
    assert(1, 2 == 2, "2 == 2");
    assert(0, 2 != 2, "2 != 2");

    assert(1, 2 + 4 * (8 + 2) == 42, "2 + 4 * (8 + 2) == 42");
    assert(0, 42 != 2 + 4 * (8 + 2), "42 != 2 + 4 * (8 + 2)");
    assert(0, 1 == 2 == 2, "1 == 2 == 2");
    assert(1, 1 == (2 == 2), "1 == (2 == 2)");
    assert(1, 2 < 3, "2 < 3");
    assert(0, 2 > 3, "2 > 3");
    assert(1, 2 <= 3, "2 <= 3");
    assert(0, 2 >= 3, "2 >= 3");
    assert(1, 2 <= 2, "2 <= 2");
    assert(1, 3 >= 3, "3 >= 3");
    assert(1, (8 + 2) / 2 + 7 < 3 * 5, "(8 + 2) / 2 + 7 < 3 * 5");
    assert(1, (8 + 2) / 2 + 7 < 3 * 5 == 1, "(8 + 2) / 2 + 7 < 3 * 5 == 1");

    assert(10, ({ int foo; int bar; foo = bar = 1; foo + bar + 8; }), "({ int foo; int bar; foo = bar = 1; foo + bar + 8; })");
    assert(1,({int foo; foo = 1; foo;}),"({int foo; foo = 1; foo;})");
    assert(10,({int foo; int bar; foo = bar = 1; foo + bar + 8;}),"({int foo; int bar; foo = bar = 1; foo + bar + 8;})");
    assert(14,({int foo; int bar; int baz; foo = 2; bar = foo + 3; baz = (foo + bar) * foo; baz;}),"({int foo; int bar; int baz; foo = 2; bar = foo + 3; baz = (foo + bar) * foo; baz;})");
    assert(1,({int foo; int bar; foo = 1; bar = foo + 2; bar < 4;}),"({int foo; int bar; foo = 1; bar = foo + 2; bar < 4;})");
    assert(3,({int a=5; int b; if (a) a = 1; b = a + 2; b;}),"({int a=5; int b; if (a) a = 1; b = a + 2; b;})");
    assert(7,({int a=5; if (a < 5) {a = 1;} else {a = 3;} a + 4;}),"({int a=5; if (a < 5) {a = 1;} else {a = 3;} a + 4;})");
    assert(5,({int i=2; while(i < 5) {i = i + 1;} i;}),"({int i=2; while(i < 5) {i = i + 1;} i;})");
    assert(10,({int i=10; while(i < 5) {i = i + 1;} i;}),"({int i=10; while(i < 5) {i = i + 1;} i;})");
    assert(45,({int a=0; int i; for(i = 0; i < 10; i = i + 1) { a = a + i;} a;}),"({int a=0; int i; for(i = 0; i < 10; i = i + 1) { a = a + i;} a;})");
    assert(10,({int i=0; for(;i < 10;) { i = i + 1;} i;}),"({int i=0; for(;i < 10;) { i = i + 1;} i;})");
    assert(3,({int x=3; int *y=&x; *y;}),"({int x=3; int *y=&x; *y;})");
    assert(3,({int x; int *y=&x; *y=3; x;}),"({int x; int *y=&x; *y=3; x;})");
    assert(4,({int *p; alloc4(&p, 1, 2, 4, 8); int *q;  q = p + 2; *q;}),"({int *p; alloc4(&p, 1, 2, 4, 8); int *q;  q = p + 2; *q;})");
    assert(8,({int *p; alloc4(&p, 1, 2, 4, 8); int *q;  q = p + 3; *q;}),"({int *p; alloc4(&p, 1, 2, 4, 8); int *q;  q = p + 3; *q;})");
    assert(2,({int *p; alloc4(&p, 1, 2, 4, 8); int *q;  q = p + 3 - 2; *q;}),"({int *p; alloc4(&p, 1, 2, 4, 8); int *q;  q = p + 3 - 2; *q;})");

    assert(4, ({sizeof(5 + 3);}),"({sizeof(5 + 3);})");
    assert(4, ({sizeof(sizeof(4));}),"({sizeof(sizeof(4));})");
    assert(4, ({int x; sizeof x;}),"({int x; sizeof x;})");
    assert(8, ({int *x; sizeof x;}),"({int *x; sizeof x;})");
    assert(4, ({int x; sizeof(x);}),"({int x; sizeof(x);})");
    assert(8, ({int x=3; sizeof(&x);}),"({int x=3; sizeof(&x);})");
    assert(8, ({int *y; sizeof(y);}),"({int *y; sizeof(y);})");
    assert(8, ({int *y; sizeof(y + 1);}),"({int *y; sizeof(y + 1);})");
    assert(4, ({int *y; sizeof(*y);}),"({int *y; sizeof(*y);})");

    assert(5, ({int x[2][4]; x[1][3] = 5; x[1][3];}),"({int x[2][4]; x[1][3] = 5; x[1][3];})");
    assert(5, ({int x[2][4]; 5;}),"({int x[2][4]; 5;})");

    assert(0, ({ int x[2][3]; int *y=x; *y=0; **x; }),"({ int x[2][3]; int *y=x; *y=0; **x; })");
    assert(1, ({ int x[2][3]; int *y=x; *(y+1)=1; *(*x+1); }),"({ int x[2][3]; int *y=x; *(y+1)=1; *(*x+1); })");
    assert(2, ({ int x[2][3]; int *y=x; *(y+2)=2; *(*x+2); }),"({ int x[2][3]; int *y=x; *(y+2)=2; *(*x+2); })");
    assert(3, ({ int x[2][3]; int *y=x; *(y+3)=3; **(x+1); }),"({ int x[2][3]; int *y=x; *(y+3)=3; **(x+1); })");
    assert(4, ({ int x[2][3]; int *y=x; *(y+4)=4; *(*(x+1)+1); }),"({ int x[2][3]; int *y=x; *(y+4)=4; *(*(x+1)+1); })");
    assert(5, ({ int x[2][3]; int *y=x; *(y+5)=5; *(*(x+1)+2); }),"({ int x[2][3]; int *y=x; *(y+5)=5; *(*(x+1)+2); })");

    assert(3, ({ int x[3]; *x=3; x[1]=4; x[2]=5; *x; }),"({ int x[3]; *x=3; x[1]=4; x[2]=5; *x; })");
    assert(4, ({ int x[3]; *x=3; x[1]=4; x[2]=5; *(x+1); }),"({ int x[3]; *x=3; x[1]=4; x[2]=5; *(x+1); })");
    assert(5, ({ int x[3]; *x=3; x[1]=4; x[2]=5; *(x+2); }),"({ int x[3]; *x=3; x[1]=4; x[2]=5; *(x+2); })");
    assert(5, ({ int x[3]; *x=3; x[1]=4; x[2]=5; *(x+2); }),"({ int x[3]; *x=3; x[1]=4; x[2]=5; *(x+2); })");
    assert(5, ({ int x[3]; *x=3; x[1]=4; 2[x]=5; *(x+2); }),"({ int x[3]; *x=3; x[1]=4; 2[x]=5; *(x+2); })");
    assert(5, ({ int x[3]; *x=3; x[1]=4; 2[x]=5; *(2+x); }),"({ int x[3]; *x=3; x[1]=4; 2[x]=5; *(2+x); })");

    assert(0, ({ int x[2][3]; int *y=x; y[0]=0; x[0][0]; }),"({ int x[2][3]; int *y=x; y[0]=0; x[0][0]; })");
    assert(1, ({ int x[2][3]; int *y=x; y[1]=1; x[0][1]; }),"({ int x[2][3]; int *y=x; y[1]=1; x[0][1]; })");
    assert(2, ({ int x[2][3]; int *y=x; y[2]=2; x[0][2]; }),"({ int x[2][3]; int *y=x; y[2]=2; x[0][2]; })");
    assert(3, ({ int x[2][3]; int *y=x; y[3]=3; x[1][0]; }),"({ int x[2][3]; int *y=x; y[3]=3; x[1][0]; })");
    assert(4, ({ int x[2][3]; int *y=x; y[4]=4; x[1][1]; }),"({ int x[2][3]; int *y=x; y[4]=4; x[1][1]; })");
    assert(5, ({ int x[2][3]; int *y=x; y[5]=5; x[1][2]; }),"({ int x[2][3]; int *y=x; y[5]=5; x[1][2]; })");

    assert(4, ({ int x; sizeof(x); }),"({ int x; sizeof(x); })");
    assert(4, ({ int x; sizeof x; }),"({ int x; sizeof x; })");
    assert(8, ({ int *x; sizeof(x); }),"({ int *x; sizeof(x); })");
    assert(16, ({ int x[4]; sizeof(x); }),"({ int x[4]; sizeof(x); })");
    assert(48, ({ int x[3][4]; sizeof(x); }),"({ int x[3][4]; sizeof(x); })");
    assert(16, ({ int x[3][4]; sizeof(*x); }),"({ int x[3][4]; sizeof(*x); })");
    assert(4, ({ int x[3][4]; sizeof(**x); }),"({ int x[3][4]; sizeof(**x); })");
    assert(5, ({ int x[3][4]; sizeof(**x) + 1; }),"({ int x[3][4]; sizeof(**x) + 1; })");
    assert(5, ({ int x[3][4]; sizeof **x + 1; }),"({ int x[3][4]; sizeof **x + 1; })");
    assert(4, ({ int x[3][4]; sizeof(**x + 1); }),"({ int x[3][4]; sizeof(**x + 1); })");

    assert(3, ({int a[10]; int x; x = 3; x;}),"({int a[10]; int x; x = 3; x;})");
    assert(3, ({int x; x = 3; int a[10]; x;}),"({int x; x = 3; int a[10]; x;})");
    assert(40, ({int a[10]; sizeof(a);}),"({int a[10]; sizeof(a);})");

    assert(7, ({int a[2]; *a = 7; }),"({int a[2]; *a = 7; })");
    assert(7, ({int a[2]; *a = 7; *a;}),"({int a[2]; *a = 7; *a;})");
    assert(3, ({int a[2]; *a = 1; *(a + 1) = 2; int *p; p = a; *p + *(p + 1);}),"({int a[2]; *a = 1; *(a + 1) = 2; int *p; p = a; *p + *(p + 1);})");

    assert(7, ({ int a[2]; a[0] = 3; a[1] = 4; a[0] + a[1]; }), "({ int a[2]; a[0] = 3; a[1] = 4; a[0] + a[1]; })");
    assert(7, ({ int a[10]; int x; x = 3; a[x + 4] = 7; a[8 - 1]; }), "({ int a[10]; int x; x = 3; a[x + 4] = 7; a[8 - 1]; })");

    // 配列のintを4バイトとして扱っていないと上書きが起きて後者が失敗する
    assert(11, ({ int a[10]; a[5] = 5; a[6] = 6; a[6] + a[5]; }), "({ int a[10]; a[5] = 5; a[6] = 6; a[6] + a[5]; })");
    assert(11, ({ int a[10]; a[6] = 6; a[5] = 5; a[6] + a[5]; }), "({ int a[10]; a[6] = 6; a[5] = 5; a[6] + a[5]; })");
    assert(1, ({ char x; sizeof(x); }), "({ char x; sizeof(x); })");
    assert(10, ({ char x[10]; sizeof(x); }), "({ char x[10]; sizeof(x); })");
    assert(1, ({ char x = 1; x; }), "({ char x = 1; x; })");
    assert(1, ({ char x = 1; char y = 2; x; }), "({ char x = 1; char y = 2; x; })");
    assert(2, ({ char x = 1; char y = 2; y; }), "({ char x = 1; char y = 2; y; })");
    assert(97, ({ "abc"[0]; }), "({ \"abc\"[0]; })");
    assert(98, ({ "abc"[1]; }), "({ \"abc\"[1]; })");
    assert(99, ({ "abc"[2]; }), "({ \"abc\"[2]; })");
    assert(0, ({ "abc"[3]; }), "({ \"abc\"[3]; })");
    assert(4, ({ sizeof("abc"); }), "({ sizeof(\"abc\"); })");
    assert(2, ({ int x = 2; { int x = 10; } x; }), "({ int x = 2; { int x = 10; } x; })");
    assert(10, ({ int x = 2; { x = 10; } x; }), "({ int x = 2; { x = 10; } x; })");

    assert(2, ({ foo(); }), "({ foo(); })");
    assert(5, ({ int a; a = 3; foo() + a; }), "({ int a; a = 3; foo() + a; })");
    assert(5, ({ bar(2, 3); }), "({ bar(2, 3); })");
    assert(21, ({ add6(1, 2, 3, 4, 5, 6); }), "({ add6(1, 2, 3, 4, 5, 6); })");
    assert(7, ({ test(4) + 3; }), "({ test(4) + 3; })");
    assert(8, ({ sub(10, 5) + 3; }), "({ sub(10, 5) + 3; })");
    assert(55, ({ fib(9); }), "({ fib(9); })");

    assert(2, ({ a=2; a; }), "({ a=2; a; })");
    assert(4, ({ sizeof(x); }), "({ sizeof(x); })");
    assert(0, ({ x; }), "({ x; })");
    assert(7, ({ x=3; y=4; x+y;}), "({ x=3; y=4; x+y;})");

    assert(0, ({arr1[0]=0; arr1[1]=1; arr1[2]=2; arr1[3]=3; arr1[0]; }), "({arr1[0]=0; arr1[1]=1; arr1[2]=2; arr1[3]=3; arr1[0]; })");
    assert(1, ({arr1[0]=0; arr1[1]=1; arr1[2]=2; arr1[3]=3; arr1[1]; }), "({arr1[0]=0; arr1[1]=1; arr1[2]=2; arr1[3]=3; arr1[1]; })");
    assert(2, ({arr1[0]=0; arr1[1]=1; arr1[2]=2; arr1[3]=3; arr1[2]; }), "({arr1[0]=0; arr1[1]=1; arr1[2]=2; arr1[3]=3; arr1[2]; })");
    assert(3, ({arr1[0]=0; arr1[1]=1; arr1[2]=2; arr1[3]=3; arr1[3]; }), "({arr1[0]=0; arr1[1]=1; arr1[2]=2; arr1[3]=3; arr1[3]; })");
    assert(6, ({x=2; y=3; c[3][2]=x*y; c[2][3]=x+y; c[3][2];}), "({x=2; y=3; c[3][2]=x*y; c[2][3]=x+y; c[3][2];})");
    assert(0, ({ ({ 0; }); }), "({ ({ 0; }); })");
    assert(2, ({ ({ 0; 1; 2; }); }), "({ ({ 0; 1; 2; }); })");
    assert(1, return_test(), "return_test()");
    assert(6, ({ ({1;}) + ({2;}) + ({3;}); }), "({ ({1;}) + ({2;}) + ({3;}); })");
    assert(3, ({ ({ int x=3; x; }); }), "({ ({ int x=3; x; }); })");

    assert(3, ({ int x=3; int y=4;  deref(&x); }), "({ int x=3; int y=4;  deref(&x); })"); // 通った。関数呼び出し時の引数はINT決め打ちだからいけたっぽい
    assert(45, ({int a; a=0; calc(10, &a); a;}), "({int a; a=0; calc(10, &a); a;})");

    assert(2, ({ sub_char(7, 2, 3); }), "({ sub_char(7, 2, 3); })");
    assert(2, ({arr1[0]=0; arr1[1]=1; arr1[2]=2; arr1[3]=3; array_access(arr1, 2); }), "({arr1[0]=0; arr1[1]=1; arr1[2]=2; arr1[3]=3; array_access(arr1, 2); })");

    // FIXME arrがなぜかエラーにならない 先頭が同じ変数が定義されていると存在しているとみなしてそう。
//     assert(2, ({arr1[0]=0; arr1[1]=1; arr1[2]=2; arr1[3]=3; array_access(arr, 2); }), "({arr1[0]=0; arr1[1]=1; arr1[2]=2; arr1[3]=3; array_access(arr, 2); })");

    assert(2, ({test_block();}), "({test_block();})");
    printNl("OK");
    return 0;
}
