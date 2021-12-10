CFLAGS:=-std=gnu11 -g -O0 -Wextra -Wall -Winline -Wshadow -fanalyzer

default:
	while true; do read -r; $(MAKE) -B a.out; printf "\e[32m%s\e[0m\n" "[$$(date +%T)]"; done

run:
	while true; do read -r; tput reset; clear; ./a.out &>/tmp/a.log; done
# 	while true; do read -r; tput reset; clear; ./a.out; done
# 	while true; do read -r; tput reset; clear; valgrind --leak-check=full --show-leak-kinds=definite --log-file="/tmp/v.log" ./a.out &>/tmp/a.log; done

a.out: LDLIBS+=$(shell pkg-config --libs cairo,librsvg-2.0)
a.out: main.o fb.o cairo.o rsvg.o
	$(CC) $(LDFLAGS) $(TARGET_ARCH) $^ $(LOADLIBES) $(LDLIBS) -o $@

fb.o:
main.o: CFLAGS+=$(shell pkg-config --cflags cairo)
cairo.o: CFLAGS+=$(shell pkg-config --cflags cairo)
rsvg.o: CFLAGS+=$(shell pkg-config --cflags librsvg-2.0)
