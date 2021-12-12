##############################################################################################

CFLAGS:=-std=gnu11 -g -O0 -Wextra -Wall -Winline -Wshadow -fanalyzer
LDLIBS:=-lgdbm

default: serialize key Cangjie5_special.gdbm Cangjie5.gdbm

clean:
	@rm -fv serialize key *.o *.gdbm *.out

##############################################################################################

key: key.o
	$(CC) -o $@ $^ $(shell pkg-config --libs ncurses) -lgdbm

key.o: CFLAGS+=$(shell pkg-config --cflags ncurses) -Wno-analyzer-use-after-free

##############################################################################################

Cangjie5_special.gdbm: Cangjie5_special.txt serialize
	./check.sh <$<
	rm -fv $@
	./serialize $@ <$<

Cangjie5.gdbm: Cangjie5.txt serialize
	cat $< | tail -n +12 | sed -Ee 's,\t\[([OuUvV!]+|MARK|o|ou|\?)\]$$,,g' | ./check.sh
	rm -fv $@
	cat $< | tail -n +12 | sed -Ee 's,\t\[([OuUvV!]+|MARK|o|ou|\?)\]$$,,g' | ./serialize $@

serialize:

##############################################################################################
