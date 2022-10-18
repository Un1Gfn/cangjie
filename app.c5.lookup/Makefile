include ../Makefile.inc

default: keyquiz.out lookup.out

clean:
	@rm -fv keyquiz.out lookup.out *.o *.gdbm *.out

keyquiz.out: keyquiz.o l2r.o
	$(CC) -o $@ $^ -lgdbm $(shell pkg-config --libs ncurses)

lookup.out: lookup.o l2r.o
	$(CC) -o $@ $^ -lgdbm $(shell pkg-config --libs readline)

keyquiz.o: CFLAGS+=$(shell pkg-config --cflags ncurses) -Wno-analyzer-use-after-free
l2r.o:
lookup.o: CFLAGS+=$(shell pkg-config --cflags readline)
