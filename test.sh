#!/bin/bash
cc -c foo.c
assert() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  cc -o tmp --static tmp.s foo.o
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 1 'int main() { char x;x=1; return x; }'
assert 1 'int main() { char x;x=1; char y;y=2; return x; }'
assert 2 'int main() { char x;x=1; char y;y=2; return y; }'

assert 1 'int main() { char x; return sizeof(x); }'
assert 10 'int main() { char x[10]; return sizeof(x); }'
assert 1 'int main() { return sub_char(7, 3, 3); } int sub_char(char a, char b, char c) { return a-b-c; }'

assert 21 "int main(){return 5+20-4;}"
assert 41 "int main(){return 12 ++ 34 - 5 ;}"
assert 8 "int main(){return  2 * 4 ;}"
assert 2 "int main(){return  4 / 2 ;}"
assert 6 "int main(){return  (4 + 2) ;}"
assert 1 "int main(){return  (21 + 3) / 8 / 3 ;}"
assert 10 "int main(){return -10 + 20 ;}"
assert 30 "int main(){return 10 - - 20 ;}"
assert 1 "int main(){return -3*+5 + 16;}"
assert 1 "int main(){return 2 == 2;}"
assert 0 "int main(){return 2 != 2;}"

assert 1 "int main(){return 2 + 4 * (8 + 2) == 42;}"
assert 0 "int main(){return 42 != 2 + 4 * (8 + 2);}"
assert 0 "int main(){return 1 == 2 == 2;}"
assert 1 "int main(){return 1 == (2 == 2);}"

assert 1 "int main(){return 2 < 3;}"
assert 0 "int main(){return 2 > 3;}"
assert 1 "int main(){return 2 <= 3;}"
assert 0 "int main(){return 2 >= 3;}"
assert 1 "int main(){return 2 <= 2;}"
assert 1 "int main(){return 3 >= 3;}"

assert 1 "int main(){return (8 + 2) / 2 + 7 < 3 * 5;}"
assert 1 "int main(){return (8 + 2) / 2 + 7 < 3 * 5 == 1;}"

assert 10 "int main(){return 10;}"
assert 1 "int main(){int foo; foo = 1; return foo;}"
assert 10 "int main(){int foo; int bar; foo = bar = 1; return foo + bar + 8;}"
assert 14 "int main(){int foo; int bar; int baz; foo = 2; bar = foo + 3; baz = (foo + bar) * foo; return baz;}"
assert 1 "int main(){int foo; int bar; foo = 1; bar = foo + 2; return bar < 4;}"

assert 1 "int main(){int a; a =5; if (a <= 5) return a = 1; return 2;}"
assert 3 "int main(){int a; int b; a =5; if (a) a = 1; b = a + 2; return b;}"
assert 7 "int main(){int a; a =5; if (a < 5) {a = 1;} else {a = 3;} return a + 4;}"

assert 5 "int main(){int i; i = 2; while(i < 5) {i = i + 1;} return i;}"
assert 10 "int main(){int i; i = 10; while(i < 5) {i = i + 1;} return i;}"

assert 45 "int main(){int a; int i; a = 0; for(i = 0; i < 10; i = i + 1) { a = a + i;} return a;}"
assert 10 "int main(){int i; i = 0; for(;i < 10;) { i = i + 1;} return i;}"

assert 2 "int main(){ return foo(); }"
assert 5 "int main(){ int a; a = 3; return foo() + a; }"

assert 5 "int main(){ return bar(2, 3); }"
assert 21 "int main(){ return add6(1, 2, 3, 4, 5, 6); }"

assert 8 "int test(){ return 5; } int main(){ return test() + 3; }"

assert 7 "int test(int input){ return input; } int main(){ return test(4) + 3; }"
assert 8 "int sub(int x, int y){ return x - y; } int main(){ return sub(10, 5) + 3; }"
assert 55 'int main(){ return fib(9); } int fib(int x){ if (x<=1) return 1; return fib(x-1) + fib(x-2); }'

assert 3 "int main(){int x; int *y; x = 3; y = &x; return *y;}"

assert 3 "int main(){ int x; int *y; y = &x; *y = 3; return x;}"
assert 3 "int main(){ int x; int *y; x = 3; return 3;}"

assert 4 "int main(){int *p; alloc4(&p, 1, 2, 4, 8); int *q;  q = p + 2; return *q;}"
assert 8 "int main(){int *p; alloc4(&p, 1, 2, 4, 8); int *q;  q = p + 3; return *q;}"
assert 2 "int main(){int *p; alloc4(&p, 1, 2, 4, 8); int *q;  q = p + 3 - 2; return *q;}"

assert 4 "int main(){return sizeof(5 + 3);}"
assert 4 "int main(){return sizeof(sizeof(4));}"
assert 4 "int main(){int x; return sizeof x;}"
assert 8 "int main(){int *x; return sizeof x;}"
assert 4 "int main(){int x; return sizeof(x);}"
assert 8 "int main(){int x; x = 3; return sizeof(&x);}"
assert 8 "int main(){int *y; return sizeof(y);}"
assert 8 "int main(){int *y; return sizeof(y + 1);}"
assert 4 "int main(){int *y; return sizeof(*y);}"

assert 5 "int main(){int x[2][4]; x[1][3] = 5; return x[1][3];}"
assert 2 "int foo; int *bar; int *baz[8][4]; int main(){foo = 2; return 2;}"
assert 2 "int foo; int *bar; int *baz[8][4]; int main(){return 2;}"
assert 5 "int main(){int x[2][4]; return 5;}"

assert 0 'int main() { int x[2][3]; int *y;y=x; *y=0; return **x; }'
assert 1 'int main() { int x[2][3]; int *y;y=x; *(y+1)=1; return *(*x+1); }'
assert 2 'int main() { int x[2][3]; int *y;y=x; *(y+2)=2; return *(*x+2); }'
assert 3 'int main() { int x[2][3]; int *y;y=x; *(y+3)=3; return **(x+1); }'
assert 4 'int main() { int x[2][3]; int *y;y=x; *(y+4)=4; return *(*(x+1)+1); }'
assert 5 'int main() { int x[2][3]; int *y;y=x; *(y+5)=5; return *(*(x+1)+2); }'

assert 3 'int main() { int x[3]; *x=3; x[1]=4; x[2]=5; return *x; }'
assert 4 'int main() { int x[3]; *x=3; x[1]=4; x[2]=5; return *(x+1); }'
assert 5 'int main() { int x[3]; *x=3; x[1]=4; x[2]=5; return *(x+2); }'
assert 5 'int main() { int x[3]; *x=3; x[1]=4; x[2]=5; return *(x+2); }'
assert 5 'int main() { int x[3]; *x=3; x[1]=4; 2[x]=5; return *(x+2); }'
assert 5 'int main() { int x[3]; *x=3; x[1]=4; 2[x]=5; return *(2+x); }'

assert 0 'int main() { int x[2][3]; int *y;y=x; y[0]=0; return x[0][0]; }'
assert 1 'int main() { int x[2][3]; int *y;y=x; y[1]=1; return x[0][1]; }'
assert 2 'int main() { int x[2][3]; int *y;y=x; y[2]=2; return x[0][2]; }'
assert 3 'int main() { int x[2][3]; int *y;y=x; y[3]=3; return x[1][0]; }'
assert 4 'int main() { int x[2][3]; int *y;y=x; y[4]=4; return x[1][1]; }'
assert 5 'int main() { int x[2][3]; int *y;y=x; y[5]=5; return x[1][2]; }'

assert 4 'int main() { int x; return sizeof(x); }'
assert 4 'int main() { int x; return sizeof x; }'
assert 8 'int main() { int *x; return sizeof(x); }'
assert 16 'int main() { int x[4]; return sizeof(x); }'
assert 48 'int main() { int x[3][4]; return sizeof(x); }'
assert 16 'int main() { int x[3][4]; return sizeof(*x); }'
assert 4 'int main() { int x[3][4]; return sizeof(**x); }'
#assert 5 'int main() { int x[3][4]; return sizeof(**x) + 1; }'
#assert 5 'int main() { int x[3][4]; return sizeof **x + 1; }'
assert 4 'int main() { int x[3][4]; return sizeof(**x + 1); }'

assert 3 "int main(){int a[10]; int x; x = 3; return x;}"
assert 3 "int main(){int x; x = 3; int a[10]; return x;}"
assert 40 "int main(){int a[10]; return sizeof(a);}"

assert 7 "int main(){int a[2]; *a = 7; }"
assert 7 "int main(){int a[2]; *a = 7; return *a;}"
assert 3 "int main(){int a[2]; *a = 1; *(a + 1) = 2; int *p; p = a; return *p + *(p + 1);}"
# そもそもこの記述って動くべきなのか？ 仕様が良く分からないのでスルー
#assert 7 "int main(){int a[2]; *a = 7; return *(&a);}"

assert 7 "int main(){int a[2]; a[0] = 3; a[1] = 4; return a[0] + a[1];}"
assert 7 "int main(){int a[10]; int x; x = 3; a[x + 4] = 7; return a[8 - 1];}"

assert 1 "int main(){int a[10]; a[0]=1; a[1]=2; return calc(a); } int calc(int *nums) { return *nums; }"
assert 2 "int main(){int a[10]; a[0]=1; a[1]=2; return calc(a); } int calc(int *nums) { return *(nums+1); }"
assert 5 "int main(){int a[10]; a[0]=1; a[1]=2; calc(a); return a[1]; } int calc(int *nums) { *(nums+1)=5; }"

# 配列のintを4バイトとして扱っていないと上書きが起きて後者が失敗する
assert 11 "int main(){int a[10]; a[5] = 5; a[6] = 6; return a[6] + a[5];}"
assert 11 "int main(){int a[10]; a[6] = 6; a[5] = 5; return a[6] + a[5];}"

# 引数、変数で確保するスタックのサイズと順序が正しくないと失敗する
assert 45 "int main(){int a; a=0; calc(10, &a); return a;} int calc(int max, int *ad) {int i;for(i=0;i<max;i=i+1){*ad=*ad+i;}}"

assert 4 "int x; int main(){return sizeof(x);}"
assert 0 'int x; int main() { return x; }'
assert 3 'int x; int main() { x=3; return x; }'
assert 7 'int x; int y; int main() { x=3; y=4; return x+y; }'
assert 0 'int x[4]; int main() { x[0]=0; x[1]=1; x[2]=2; x[3]=3; return x[0]; }'
assert 1 'int x[4]; int main() { x[0]=0; x[1]=1; x[2]=2; x[3]=3; return x[1]; }'
assert 2 'int x[4]; int main() { x[0]=0; x[1]=1; x[2]=2; x[3]=3; return x[2]; }'
assert 3 'int x[4]; int main() { x[0]=0; x[1]=1; x[2]=2; x[3]=3; return x[3]; }'
assert 6 "int foo; int *bar; int *baz[8][4]; int main(){foo=2; bar=3; baz[3][2]=foo*bar; baz[2][3]=foo+bar; return baz[3][2];}"

echo OK
