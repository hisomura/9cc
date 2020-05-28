play() {
  src="$1"
  ./9cc "playground/$src" > tmp_play.s
  cc -c foo.c
  cc -o tmp --static tmp_play.s foo.o
  ./tmp
}

play fizzbuzz.c

