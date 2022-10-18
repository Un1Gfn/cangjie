include ../Makefile.inc

default: mkdict mkdict key Cangjie5_special.gdbm Cangjie5.gdbm

clean:
	@rm -fv mkdict lookup key *.o *.gdbm *.out

##############################################################################################

lookup: lookup.o l2r.o
	$(CC) -o $@ $^ -lgdbm $(shell pkg-config --libs readline)

lookup.o: CFLAGS+=$(shell pkg-config --cflags readline)
l2r.o:

##############################################################################################

key: key.o l2r.o
	$(CC) -o $@ $^ -lgdbm $(shell pkg-config --libs ncurses)

key.o: CFLAGS+=$(shell pkg-config --cflags ncurses) -Wno-analyzer-use-after-free

##############################################################################################

Cangjie5_special.gdbm: Cangjie5_special.txt mkdict
	./check.sh <$<
	rm -fv $@
	./mkdict $@ <$<

Cangjie5.gdbm: Cangjie5.txt mkdict
	cat $< | tail -n +12 | sed -Ee 's,\t\[([OuUvV!]+|MARK|o|ou|\?)\]$$,,g' | ./check.sh
	rm -fv $@
	cat $< | tail -n +12 | sed -Ee 's,\t\[([OuUvV!]+|MARK|o|ou|\?)\]$$,,g' | ./mkdict $@

mkdict:

##############################################################################################
