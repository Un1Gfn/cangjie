CFLAGS:=-std=gnu11 -g -O0 -Wextra -Wall -Winline -Wshadow -fanalyzer

default:
	while true; do read -r; $(MAKE) -B a.out; printf "\e[32m%s\e[0m\n" "[$$(date +%T)]"; done

run:
	tput reset; clear; env G_DEBUG=fatal-warnings ./a.out 2>/tmp/a.log
# 	tput reset; clear; env G_DEBUG=fatal-warnings valgrind --leak-check=full --show-leak-kinds=definite --log-file="/tmp/v.log" ./a.out 2>/tmp/a.log

clean:
	rm -fv *.o a.out dict.tmp.c

a.out: LDLIBS+=$(shell pkg-config --libs cairo,librsvg-2.0)
a.out: main.o fb.o cairo.o rsvg.o dict.tmp.o termios.o
	$(CC) $(LDFLAGS) $(TARGET_ARCH) $^ $(LOADLIBES) $(LDLIBS) -o $@

fb.o:
main.o: CFLAGS+=$(shell pkg-config --cflags cairo)
cairo.o: CFLAGS+=$(shell pkg-config --cflags cairo)
rsvg.o: CFLAGS+=$(shell pkg-config --cflags librsvg-2.0)
dict.tmp.o:
termios.o:

dict.tmp.c: dict.sh
	shellcheck dict.sh # || true
	./dict.sh 1>|dict.tmp.c
