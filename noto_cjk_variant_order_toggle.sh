#!/bin/bash

set -e
function err {
  R="$?"
  if [ "$R" -ne 0 ]; then
    echo "err" "$1"$'\n'"$2()"$'\n'"'$3'"
    exit 2
  fi
  echo "OK"
  exit 0
}
trap 'err "${BASH_SOURCE[0]}" "${FUNCNAME[0]}" "$BASH_COMMAND"' EXIT
trap 'echo "SIGINT"; exit 0' SIGINT

TARGET=/etc/fonts/conf.avail/64-language-selector-prefer.conf
LINK=/etc/fonts/conf.d/64-language-selector-prefer.conf

function confirm {
  test "$#" -eq 1
  echo -n "$1 ?"
  read -r
}

if [ -e "$LINK" ]; then
  test -L "$LINK"
  confirm "tc first -> ja first"
  sudo rm -v "$LINK"
  sudo fc-cache --force --verbose
  fc-match --sort | grep --fixed-strings --regexp='NotoSansCJK-Regular.ttc: "Noto Sans CJK JP" "Regular"'
else
  confirm "ja first -> tc first"
  sudo ln -sv "$TARGET" "$LINK"
  sudo fc-cache --force --verbose
  fc-match --sort | grep --fixed-strings --regexp='NotoSansCJK-Regular.ttc: "Noto Sans CJK TC" "Regular"'
fi

echo
echo 'https://wiki.archlinux.org/index.php/Localization/Simplified_Chinese#Fixed_Simplified_Chinese_display_as_a_variant_(Japanese)_glyph'
echo
echo "Please restart Chromium"
echo

