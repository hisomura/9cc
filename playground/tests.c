int main() {

    assert(21, 5 + 20 - 4, "5+20-4");
    assert(8, 2 * 4, " 2 * 4 ");
    assert(1, 4 / 3, " 4 / 3 ");
    assert(6, (4 + 2), " (4 + 2) ");
    assert(1, (21 + 3) / 8 / 3, " (21 + 3) / 8 / 3 ");
    assert(10, -10 + 20, "-10 + 20 ");
    assert(30, 10 - -20, "10 - - 20 ");
    assert(1, -3 * +5 + 16, "-3*+5 + 16");
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


    printNl("OK");
    return 0;
}
