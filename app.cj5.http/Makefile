################################################################################

# 'meson compile' can also accept TARGET(s)
# N:=meson compile -v -C $(O)
N:=ninja -j4 -v -C $(O)
A:=$(shell uname -m)
O:=builddir.$(A)
S:=srv.$(A)

# H:=wt88047wlan0
H:=wt88047usb
E:=enp0s20f0u2

################################################################################

default: install

entr:
	@printf "\e]0;%s\a" "app.jc5.http (make)"
# 	https://stackoverflow.com/a/16595367/
# 	@find . -not \( -path $(O) -prune \) -type f -name '*.c' -o -name '*.h'
	@find . -not \( -path $(O) -prune \) -type f -name '*.c' -o -name '*.h' | entr $(MAKE) install

cscope:
	@echo "usage: make cscope ID=<identifier>"
	cscope -1 $(shell pkg-config --cflags-only-I kcgi{,-html}) $(ID) main.c

################################################################################

transfer: builddir.aarch64/srv.aarch64
	[ x86_64 = $(A) ]
	ip -o -4 addr show dev $(E) | grep inet >/dev/null || sudo dhcpcd $(E)
	@:
	ssh root@$(H) rm -rfv /srv.cj5
	scp -r $< root@$(H):/srv.cj5
	ssh root@$(H) chown -Rv nobody:nobody /srv.cj5
	ssh root@$(H) find /srv.cj5
	@:
	scp 99_http.nft root@$(H):/etc/etc/nftables.d
	scp wt88047.srv.cj5.sh root@$(H):/srv.cj5.sh
	ssh root@$(H) chmod +x /srv.cj5.sh
	ssh root@$(H) rc-service restart nftables
	ssh root@$(H) nft list ruleset

# reinstall to generate logs for uninstallation
uninstall:
	@rm -rfv $(O)/$(S)/*
# 	@$(N)
# 	@meson install -C $(O)
# 	env DESTDIR='$(shell pwd)/$(O)' $(N) uninstall
# 	/usr/bin/meson --internal uninstall -C $(O) --destdir .

install: | $(O)
	@printf "\e[7m%s\e[0m\n" "$(shell date)"
	@meson install -C $(O) --destdir .
# 	@$(N)

# order-only prerequisite
# build on nonexistence only, no rebuild on outdate
build: | $(O)
	@printf "\e[7m%s\e[0m\n" "$(shell date)"
	$(N)

# setup
$(O):
	@echo "/Makefile --> /meson.build --> /$(O)/build.ninja"
	@rm -rf $(O)
	meson setup -D prefix=/$(S) $(O) .

################################################################################

# https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Makefile?h=v5.19#n1561

clean:
	@{ [ -e $(O) ] && $(N) -t clean; } || true
	@rm -fv cscope.out

# mrproper:
# 	@echo "$@ not allowed"; /bin/false

distclean:
	@$(MAKE) clean
	@rm -rfv $(O)/

# purge:
# 	@read -rp "uninstall? "
# 	@$(MAKE) 2_uninstall
# 	@$(MAKE) clean
# 	@rm -rfv $(O)/

################################################################################
