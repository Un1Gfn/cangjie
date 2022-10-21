O:=builddir
N:=ninja -j4 -v -C $(O)
IN?=sample.txt
OUT?=/dev/stdout

default: 2_entr

$(O):
	@echo "/Makefile --> /meson.build --> /builddir/build.ninja"
	@rm -rf $(O)
	meson setup $(O) .

2_entr:
# 	https://stackoverflow.com/a/16595367/
# 	@find . -not \( -path ./builddir -prune \) -type f -name '*.c' -o -name '*.h'
	@find . -not \( -path ./builddir -prune \) -type f -name '*.c' -o -name '*.h' | entr $(MAKE) 1_install

1_install:
	@printf "\e[7m%s\e[0m\n" "$(shell date)"
	@meson install -C $(O)
# 	@echo $$RANDOM

# order-only prerequisite
# build on nonexistence only, no rebuild on outdate
0_build: | $(O)
# 	meson compile -v -C $(O)
	$(N)

# https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Makefile?h=v5.19#n1561
clean:
	@{ [ -e $(O) ] && $(N) -t clean; } || true
	@rm -fv cscope.out
mrproper:
	@rm -rfv $(O)/
distclean: mrproper
