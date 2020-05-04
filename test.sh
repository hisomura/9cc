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
#assert 0 0
#assert 42 42
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
assert 14 " foo = 2; bar = foo + 3; baz = (foo + bar) * foo; return baz;"

echo OK
