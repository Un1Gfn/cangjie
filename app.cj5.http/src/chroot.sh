#!/bin/bash

# for how to assemble chroot, refer to sphinx.public/WT88047/postmarketOS/APKBUILD
# http://127.240.9.239:58250/postmarketos.html#apkbuild

err(){
  printf "\e[31m%s\e[0m\n" "$1"
  exit
}

{

  echo

  F1=/home/darren/cangjie/app.cj5.http/src
  T1=/home/darren/wt88047.pmbootstrap/chroot_buildroot_aarch64/home/darren/cj5.bindmount

  mkdir -p "$T1"
  [ -z "$(ls -A "$T1")" ] || err "dest dir not empty"
  sudo mount -v --bind "$F1" "$T1" || err "bind mount failed"
  alacrittytitle.sh 'darren@chroot_buildroot_aarch64'
  echo

  echo ":; su - darren"
  echo ":; cd ~/cj5.bindmount"
  echo ":; make distclean default"
  echo "## make t "
  echo

  pmbootstrap chroot -b aarch64
  echo

  UMOUNT=(sudo umount -v "$T1")
  SHUTDOWN=(pmbootstrap shutdown)
  echo ":; ${UMOUNT[@]}"
  echo
  echo ":; ${SHUTDOWN[@]}"
  echo
  "${UMOUNT[@]}" || err "unmount failed"
  "${SHUTDOWN[@]}" || err "shutdown failed"

}; exit
