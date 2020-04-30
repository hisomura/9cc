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

assert 1 " a = 1;"
assert 10 " a = 1; b = a + 9;"
assert 10 " a = b = 1; a + b + 8;"
assert 1 " a = 1; c = a + 2; c < 4;"
echo OK
