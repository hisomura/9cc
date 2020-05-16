#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  cc -o tmp tmp.s
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

assert 21 "5+20-4;"
assert 41 " 12 ++ 34 - 5 ;"
assert 8 "  2 * 4 ;"
assert 2 "  4 /    2 ;"
assert 6 "  (4 + 2) ;"
assert 1 "  (21 + 3) / 8 / 3 ;"
assert 10 " -10 + 20 ;"
assert 30 " 10 - - 20 ;"
assert 1 " -3*+5 + 16;"
assert 1 " 2 == 2;"
assert 0 " 2 != 2;"

assert 1 " 2 + 4 * (8 + 2) == 42;"
assert 0 " 42 != 2 + 4 * (8 + 2);"
assert 0 " 1 == 2 == 2;"
assert 1 " 1 == (2 == 2);"

assert 1 " 2 < 3;"
assert 0 " 2 > 3;"
assert 1 " 2 <= 3;"
assert 0 " 2 >= 3;"
assert 1 " 2 <= 2;"
assert 1 " 3 >= 3;"

assert 1 " (8 + 2) / 2 + 7 < 3 * 5;"
assert 1 " (8 + 2) / 2 + 7 < 3 * 5 == 1;"

assert 1 " foo = 1;"
assert 10 " foo = 1; bar = foo + 9;"
assert 10 " foo = bar = 1; foo + bar + 8;"
assert 1 " foo = 1; bar = foo + 2; bar < 4;"
assert 6 " foo = 1; bar = 2 + 3; foo + bar;"

assert 10 "return 10;"
assert 2 "return b = 2;"
assert 14 " foo = 2; bar = foo + 3; baz = (foo + bar) * foo; return baz;"

assert 1 "a =5; if (a <= 5) a = 1;"
#assert 5 "a =5; if (a < 5) return a = 1;"
assert 3 "a =5; if (a) a = 1; b = a + 2;"
assert 7 "a =5; if (a < 5) a = 1; else a = 3; return b = a + 4;"

assert 5 "i = 2; while(i < 5) i = i + 1; return i;"
assert 10 "i = 10; while(i < 5) i = i + 1; return i;"

assert 45 "a = 0; for(i = 0; i < 10; i = i + 1)  a = a + i; return a;"
assert 10 "i = 0; for(;i < 10;)  i = i + 1; return i;"
echo OK
