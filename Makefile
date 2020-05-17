CFLAGS=-std=c11 -g -static
SRCS=$(filter-out foo.c, $(wildcard *.c))
OBJS=$(addprefix obj/, $(SRCS:.c=.o))

9cc: $(OBJS)
	$(CC) -o 9cc $(OBJS) $(LDFLAGS)

$(OBJS): 9cc.h $(SRCS)
	$(CC) $(CFLAGS) -c -o $@ $(subst obj/,,$(@:.o=.c))

test: 9cc
	./test.sh

clean:
	rm -f 9cc obj/*.o *~ tmp*

.PHONY: test clean