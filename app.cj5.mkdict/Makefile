include ../Makefile.inc

default: mkdict.out Cangjie5_special.gdbm Cangjie5.gdbm

clean:
	@rm -fv mkdict.out *.o *.gdbm *.out

Cangjie5_special.gdbm: Cangjie5_special.txt mkdict.out
	./check.sh <$<
	rm -fv $@
	./mkdict.out $@ <$<

Cangjie5.gdbm: Cangjie5.txt mkdict.out
	cat $< | tail -n +12 | sed -Ee 's,\t\[([OuUvV!]+|MARK|o|ou|\?)\]$$,,g' | ./check.sh
	rm -fv $@
	cat $< | tail -n +12 | sed -Ee 's,\t\[([OuUvV!]+|MARK|o|ou|\?)\]$$,,g' | ./mkdict.out $@

mkdict.out:
