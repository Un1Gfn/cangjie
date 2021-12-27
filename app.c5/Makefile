##############################################################################################

# #include <uchar.h>
# -fextended-identifiers for \unnnn and \U00nnnnnn notations
CFLAGS:=-std=gnu11 -g -O0 -Wextra -Wall -Winline -Wshadow -fanalyzer -fextended-identifiers
LDLIBS:=-lgdbm

default: dict serialize key Cangjie5_special.gdbm Cangjie5.gdbm

clean:
	@rm -fv serialize key *.o *.gdbm *.out

##############################################################################################

dict: dict.o l2r.o
	$(CC) -o $@ $^ -lgdbm $(shell pkg-config --libs readline)

dict.o: CFLAGS+=$(shell pkg-config --cflags readline)
l2r.o:

##############################################################################################

key: key.o l2r.o
	$(CC) -o $@ $^ -lgdbm $(shell pkg-config --libs ncurses)

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
