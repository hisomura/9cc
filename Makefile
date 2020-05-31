CFLAGS=-std=c11 -g -static
SRCS=$(filter-out foo.c tmp.c, $(wildcard *.c))
OBJS=$(addprefix obj/, $(SRCS:.c=.o))

9cc: $(OBJS)
	$(CC) -o 9cc $(OBJS) $(LDFLAGS)

$(OBJS): 9cc.h $(SRCS)
	$(CC) $(CFLAGS) -c -o $@ $(subst obj/,,$(@:.o=.c))

test: 9cc
	./9cc tests/tests.c > tmp_tests.s
	$(CC) -c foo.c
	$(CC) --static -o tmp tmp_tests.s foo.o
	./tmp

play: 9cc
	./play.sh

clean:
	rm -f 9cc obj/*.o *~ tmp* foo.o

.PHONY: test clean