#!/bin/bash

{

  IPP="127.11.111.201:9513"

  ARGS=(
    -f
    -vv
    -p "$IPP"
    # -u darren:darren
    -h /home/darren/cangjie/app.c5.srv
    # -c /etc/httpd.conf
  )

  [ "darren" = "$(whoami)" ] || exit 1

  echo "http://$IPP"
  busybox httpd "${ARGS[@]}"

}; exit

