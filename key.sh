#!/bin/bash


{

  trap "tput cnorm; clear; exit;" SIGINT
  tput civis
  clear

  echo

  printf "\e[33m%s\e[0m %s" "倉[人戈日口]?"
  echo
  printf ">"
  printf " \e[32m%s\e[0m" 人 戈
  printf " \e[31m%s\e[0m" 尸
  echo
  printf ">"
  printf " \e[32m%s\e[0m" 人 戈 日 口
  echo

  printf "\e[33m%s\e[0m %s" "麻[戈日][戈木木]?"
  echo
  printf ">"
  printf " \e[32m%s\e[0m" 戈 日
  echo

  printf "\e[33m%s\e[0m %s" "麻[戈日][戈木木]?"
  echo
  printf ">"
  printf " \e[32m%s\e[0m" 戈 木 木
  echo

  # printf "\e[2m%s\e[0m" "[press Q]"

  # move cursor to bottom
  printf "\e[$(tput lines);1H"

  # htop(1) style
  # printf "\e[7m%s\e[0m" "Q"
  printf " "
  printf F5
  printf "\e[2m%s\e[0m" / # \|
  printf ^R
  printf "\e[2m%s\e[0m" / # \|
  printf "\e[44m%-7s\e[0m" Resize

  printf " "
  printf F10
  printf "\e[2m%s\e[0m" / # \|
  printf BKSP
  printf "\e[2m%s\e[0m" / # \|
  printf Enter
  printf "\e[2m%s\e[0m" / # \|
  printf "\e[44m%-7s\e[0m" Quit

  printf "\e[$(tput lines);1H"

  read -r
  tput cnorm
  clear

}; exit
