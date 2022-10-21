################################################################################

O:=builddir
# 'meson compile' can also accept TARGET(s)
# B:=meson compile -v -C $(O)
B:=ninja -j4 -v -C $(O)
IN?=sample.txt
OUT?=/dev/stdout

################################################################################

default: 1_install

entr:
	@printf "\e]0;%s\a" "app.jc5.http (make)"
# 	https://stackoverflow.com/a/16595367/
# 	@find . -not \( -path ./builddir -prune \) -type f -name '*.c' -o -name '*.h'
	@find . -not \( -path ./builddir -prune \) -type f -name '*.c' -o -name '*.h' | entr $(MAKE) 1_install

cscope:
	@echo "usage: make cscope ID=<identifier>"
	cscope -1 $(shell pkg-config --cflags-only-I kcgi{,-html}) $(ID) main.c

################################################################################

# reinstall to generate logs for uninstallation
2_uninstall: | $(O)
	@$(B)
	@meson install -C $(O)
	@$(B) uninstall

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

purge:
	@read -rp "uninstall? "
	@$(MAKE) 2_uninstall
	@$(MAKE) clean
	@rm -rfv $(O)/

distclean:
	@$(MAKE) clean
	@rm -rfv $(O)/

# mrproper:
# 	@echo "$@ not allowed"; /bin/false

clean:
	@{ [ -e $(O) ] && $(B) -t clean; } || true
	@rm -fv cscope.out

################################################################################
