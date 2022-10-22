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

  printf "\e]0;%s\a" "app.jc5.http (httpd)"

  cd "$SRV"

  echo "http://$IPP/cgi-bin/cj5.cgi         (200, implicit)"
  echo "http://$IPP/cgi-bin/cj5.cgi/        (404, cgi not called at all)"
  echo "http://$IPP/cgi-bin/cj5.cgi/defpage (200, explicit)"
  echo

  echo "${CMD[*]}"
  "${CMD[@]}"
  echo

  printf "\e]0;%s\a" "Alacritty"

}; exit

