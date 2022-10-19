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

  echo

  cd ~/cangjie/app.c5.srv

  # for CGI in cgi-bin/*.cgi; do
  #   echo "http://$IPP/$CGI"
  # done
  echo "http://$IPP/cgi-bin/02-sample.cgi"
  echo "http://$IPP/03-tutorial.html"
  echo

  busybox httpd "${ARGS[@]}"
  echo

}; exit

