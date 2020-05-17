#!/bin/bash
cc -c foo.c
assert() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  cc -o tmp tmp.s foo.o
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

make 

assert 21 "main(){5+20-4;}"
assert 41 "main(){12 ++ 34 - 5 ;}"
assert 8 "main(){ 2 * 4 ;}"
assert 2 "main(){ 4 / 2 ;}"
assert 6 "main(){ (4 + 2) ;}"
assert 1 "main(){ (21 + 3) / 8 / 3 ;}"
assert 10 "main(){-10 + 20 ;}"
assert 30 "main(){10 - - 20 ;}"
assert 1 "main(){-3*+5 + 16;}"
assert 1 "main(){2 == 2;}"
assert 0 "main(){2 != 2;}"

assert 1 "main(){2 + 4 * (8 + 2) == 42;}"
assert 0 "main(){42 != 2 + 4 * (8 + 2);}"
assert 0 "main(){1 == 2 == 2;}"
assert 1 "main(){1 == (2 == 2);}"

assert 1 "main(){2 < 3;}"
assert 0 "main(){2 > 3;}"
assert 1 "main(){2 <= 3;}"
assert 0 "main(){2 >= 3;}"
assert 1 "main(){2 <= 2;}"
assert 1 "main(){3 >= 3;}"

assert 1 "main(){(8 + 2) / 2 + 7 < 3 * 5;}"
assert 1 "main(){(8 + 2) / 2 + 7 < 3 * 5 == 1;}"

assert 1 "main(){foo = 1;}"
assert 10 "main(){foo = 1; bar = foo + 9;}"
assert 10 "main(){foo = bar = 1; foo + bar + 8;}"
assert 1 "main(){foo = 1; bar = foo + 2; bar < 4;}"
assert 6 "main(){foo = 1; bar = 2 + 3; foo + bar;}"

assert 10 "main(){return 10;}"
assert 2 "main(){return b = 2;}"
assert 14 "main(){foo = 2; bar = foo + 3; baz = (foo + bar) * foo; return baz;}"

assert 1 "main(){a =5; if (a <= 5) a = 1;}"
#assert 5 "a =5; if (a < 5) return a = 1;}"
assert 3 "main(){a =5; if (a) a = 1; b = a + 2;}"
assert 7 "main(){a =5; if (a < 5) {a = 1;} else {a = 3;} return b = a + 4;}"

assert 5 "main(){i = 2; while(i < 5) {i = i + 1;} return i;}"
assert 10 "main(){i = 10; while(i < 5) {i = i + 1;} return i;}"

assert 45 "main(){a = 0; for(i = 0; i < 10; i = i + 1) { a = a + i;} return a;}"
assert 10 "main(){i = 0; for(;i < 10;) { i = i + 1;} return i;}"

assert 2 "main(){ return foo(); }"
assert 5 "main(){ a = 3; return foo() + a; }"

assert 5 "main(){ return bar(2, 3); }"
assert 21 "main(){ return add6(1, 2, 3, 4, 5, 6); }"

assert 8 "test() { return 5; } main(){ return test() + 3; }"

assert 7 "test(input) { return input; } main(){ return test(4) + 3; }"
assert 8 "sub(x, y) { return x - y; } main(){ return sub(10, 5) + 3; }"
assert 55 'main() { return fib(9); } fib(x) { if (x<=1) return 1; return fib(x-1) + fib(x-2); }'

echo OK
