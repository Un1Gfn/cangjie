CFLAGS:=-std=gnu17 -g -O0 -Wextra -Wall -Winline -Wshadow -fanalyzer
LDLIBS:=$(shell pkg-config --libs kcgi{,-html})
ITEMS:=01-lookup 02-sample 03-tutorial

default: $(addsuffix .cgi,$(ITEMS))
	@echo $$RANDOM

entr:
	{ for i in $(ITEMS); do echo $${i}.c; done; } | entr $(MAKE)

clean:
	@rm -fv *.o *.cgi

%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $^

%.cgi: %.o
	$(CC) -o $@ $^ $(LDLIBS)

cscope:
	@echo "usage: make cscope ID=<identifier>"
	cscope -1 $(shell pkg-config --cflags-only-I kcgi{,-html}) $(ID) 02-demo.c
