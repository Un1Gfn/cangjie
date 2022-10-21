#!/bin/bash

{

  IPP="127.11.111.201:9513"
  SRV=/home/darren/cangjie/app.cj5.http/srv
  CMD=(
    busybox httpd
    -f
    -vv
    -p "$IPP"
    # -u darren:darren
    -h "$SRV"
    # -c /etc/httpd.conf
  )

  [ "darren" = "$(whoami)" ] || exit 1

  echo

  cd "$SRV"

  echo "http://$IPP/cgi-bin/cj5.cgi/index.html"
  echo

  echo "${CMD[*]}"
  "${CMD[@]}"
  echo

}; exit

