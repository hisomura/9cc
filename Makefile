CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(addprefix obj/, $(SRCS:.c=.o))

9cc: $(OBJS)
	$(CC) -o 9cc $(OBJS) $(LDFLAGS)

$(OBJS): 9cc.h
	$(CC) $(CFLAGS) -c -o $@ $(subst obj/,,$(@:.o=.c))

test: 9cc
	./test.sh

clean:
	rm -f 9cc obj/*.o *~ tmp*

.PHONY: test clean