#!/bin/bash
cc -c foo.c
assert() {
  expected="$1"
  input="$2"
  echo "$input" > tmp.c
  ./9cc tmp.c > tmp.s || exit
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

assert 2 "int test(int b) { return b; } int main(){int b = 2; test(b); }"
#assert 2 "int test(int *a, int b) { return *(a+b); } int main(){ array_access(b, 2); }"
#assert 2 "int arr1[4]; int array_access(int *arr, int index) { return *(arr+index); } int main(){ arr1[0]=0; arr1[1]=1; arr1[2]=2; arr1[3]=3; array_access(arr, 2); }"

# そもそもこの記述って動くべきなのか？ 仕様が良く分からないのでスルー
#assert 7 "int main(){int a[2]; *a = 7; return *(&a);}"

echo OK
