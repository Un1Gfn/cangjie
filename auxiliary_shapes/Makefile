CFLAGS:=-std=gnu11 -g -O0 -Wextra -Wall -Winline -Wshadow -fanalyzer

default:
	while true; do read -r; $(MAKE) -B a.out; printf "\e[32m%s\e[0m\n" "[$$(date +%T)]"; done

run:
	while true; do read -r; tput reset; clear; ./a.out &>/tmp/a.log; done
# 	while true; do read -r; tput reset; clear; ./a.out; done
# 	while true; do read -r; tput reset; clear; valgrind ./a.out; done

a.out: LDLIBS+=$(shell pkg-config --libs cairo)
a.out: main.o fb.o cairo.o
	$(CC) $(LDFLAGS) $(TARGET_ARCH) $^ $(LOADLIBES) $(LDLIBS) -o $@

fb.o:
main.o:
cairo.o: CFLAGS+=$(shell pkg-config --cflags cairo)
