#!/bin/ash

{

  IPP=192.168.0.136:80
  SRV=/srv.cj5

  echo "http://$IPP/cgi-bin/cj5.cgi"

  sudo /bin/busybox-extras httpd \
    -f \
    -vv \
    -p "$IPP" \
    -u nobody:nobody \
    -h "$SRV"

    # -c /etc/httpd.conf

}; exit
