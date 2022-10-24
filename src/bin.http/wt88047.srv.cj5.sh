#!/bin/bash

{

  # https://askubuntu.com/questions/844913/how-to-control-ip-ranges-of-network-managers-hotspots
  # https://people.freedesktop.org/~lkundrak/nm-docs/nm-settings.html

  # IPP=192.168.0.136:80 # Tenda

  IPP=10.42.0.1:80 # Hotspot

  # address-data=[{address: '10.42.0.1', prefix: 24}]
  # address1=10.42.0.1/24,10.42.0.1
  # gateway=10.42.0.1

  # address-data=[{address: '10.44.0.1', prefix: 24}]
  # address1=10.44.0.1/24,10.44.0.4
  # gateway=10.44.0.4

  SRV=/srv.cj5

  echo "http://$IPP/cgi-bin/cj5.cgi"

  cd "$SRV" || exit 1


  CMD=(
    sudo /bin/busybox-extras httpd
    -f
    -vv
    -p "$IPP"
    -u nobody:nobody
    -h "$SRV"
    # -c /etc/httpd.conf
  )

  tmux new -A -s "cj5" "${CMD[*]}"

}; exit
