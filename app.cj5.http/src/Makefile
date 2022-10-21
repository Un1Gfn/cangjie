O:=builddir
# 'meson compile' can also accept TARGET(s)
# B:=meson compile -v -C $(O)
B:=ninja -j4 -v -C $(O)
IN?=sample.txt
OUT?=/dev/stdout

################################################################################

default: 1_install

2_entr:
# 	https://stackoverflow.com/a/16595367/
# 	@find . -not \( -path ./builddir -prune \) -type f -name '*.c' -o -name '*.h'
	@find . -not \( -path ./builddir -prune \) -type f -name '*.c' -o -name '*.h' | entr $(MAKE) 1_install

################################################################################

1_install: | $(O)
	@printf "\e[7m%s\e[0m\n" "$(shell date)"
	@$(B)
	@meson install -C $(O)

# order-only prerequisite
# build on nonexistence only, no rebuild on outdate
0_build: | $(O)
	@printf "\e[7m%s\e[0m\n" "$(shell date)"
	$(B)

# setup
$(O):
	@echo "/Makefile --> /meson.build --> /builddir/build.ninja"
	@rm -rf $(O)
	meson setup $(O) .

################################################################################

# https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Makefile?h=v5.19#n1561
clean:
	@{ [ -e $(O) ] && $(B) -t clean; } || true
	@rm -fv cscope.out
mrproper:
	@rm -rfv $(O)/
distclean: mrproper

cscope:
	@echo "usage: make cscope ID=<identifier>"
	cscope -1 $(shell pkg-config --cflags-only-I kcgi{,-html}) $(ID) main.c
