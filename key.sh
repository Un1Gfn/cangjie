#!/bin/bash
{

  tput civis
  clear

  echo

cat <<"EOF"
practice keys and radicals of Cangjie input method
<https://en.wikipedia.org/wiki/Cangjie_input_method#Keys_and_%22radicals%22>

keyboard layout
page 4 of ~/pdf/5cjbook.pdf
<https://en.wikipedia.org/wiki/Cangjie_input_method#Keyboard_layout>
EOF

  echo

  printf "\e[34m%s\e[0m" "press <Q> to quit"
  echo
  echo

  printf "\e[2m%s\e[0m" "[mistake 日_A with 斜(竹)_H 人_S 側(尸)_S, then press 日_A correctly]"
  echo
  printf "\e[33m%s \e[0m" "日?"
  printf "\e[31m%s \e[0m" "斜(竹)"
  printf "\e[31m%s \e[0m" "人"
  printf "\e[32m%s \e[0m" "日"
  echo
  echo

  printf "\e[2m%s\e[0m" "[press Q]"
  echo
  printf "\e[33m%s \e[0m" "水?"
  echo "bye"
  echo

  # move cursor to bottom
  printf "\e[$(tput lines);1H"

  # htop(1) style
  # printf "\e[7m%s\e[0m" "Q"
  printf "%s\e[44m%-6s\e[0m" F10   Quit
  printf "%s\e[44m%-6s\e[0m" Enter Quit
  printf "%s\e[44m%-6s\e[0m" BKSP  Quit

  printf "\e[$(tput lines);1H"

  read -r
  tput cnorm
  clear

}; exit
