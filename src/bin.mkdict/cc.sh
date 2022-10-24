#!/dev/null
# explicit 'bash -? -? ??.sh' 

# Cangjie5_special.gdbm: mkdict.out Cangjie5_special.txt
#   ./check.sh <$<
#   ./mkdict.out $@ <$<
# #   ./cc.sh modeA mkdict.out Cangjie5_special.txt Cangjie5_special.gdbm

# Cangjie5.gdbm: mkdict.out Cangjie5.txt
#   cat $< | tail -n +12 | sed -Ee 's,\t\[([OuUvV!]+|MARK|o|ou|\?)\]$$,,g' | ./check.sh
#   cat $< | tail -n +12 | sed -Ee 's,\t\[([OuUvV!]+|MARK|o|ou|\?)\]$$,,g' | ./mkdict.out $@
# #   ./cc.sh modeB mkdict.out Cangjie5.txt Cangjie5.gdbm

function fulltool(){
  bash --version | grep 'GNU bash'
  cat  --version | grep 'GNU coreutils'
  grep --version | grep 'GNU grep'
  sed  --version | grep 'GNU sed'
  tail --version | grep 'GNU coreutils'
}

# stdin
function check(){
  grep -v -E -e '^.+'$'\t''[a-z]+$' || R="$?"
  [ 1 = "$R" ] || {
    grep -v -E -e '^.+'$'\t''[a-z]+$' | cat -ET
    exit 1
  }
}

# take stdin
function chop(){
  tail -n +12 | sed -Ee 's,\t\[([OuUvV!]+|MARK|o|ou|\?)\]$$,,g'
}

function help(){
  echo "$0 <modeA|modeB> <mkdict_executable> <in.txt> <out.gdbm>"
}

{

  fulltool

  M="$1"
  E="$2"
  I="$3"
  O="$4"

  case "$M" in
    '-h'|'--help')
      help
      ;;
    'modeA')
      check <"$I"
      rm -fv "$O"
      "$E" "$O" <"$I"
      ;;
    'modeB')
      chop <"$I" | check
      rm -fv "$O"
      chop <"$I" | "$E" "$O"
      ;;
  esac

}; exit
