#!/bin/bash

# 日月金木水火土 竹戈十大中一弓 人心手口 尸廿山女田卜
# abcdefg hijklmn opqr stuvwy

# abcdefghijklmnopqrstuvwy
# 日月金木水火土竹戈十大中一弓人心手口尸廿山女田卜

TITLE="Rime倉頡字典"

# set -e
# function err {
#   R="$?"
#   if [ "$R" -ne 0 ]; then
#     # echo "err" "$1 $2() $3"
#     notify-send "err" "$1"$'\n'"$2()"$'\n'"'$3'"
#     exit 2
#   fi
#   # echo "OK"
#   exit 0
# }
# trap 'err "${BASH_SOURCE[0]}" "${FUNCNAME[0]}" "$BASH_COMMAND"' EXIT
# trap 'echo "SIGINT"; exit 0' SIGINT

function convert {
  test "$#" -eq 1 -a -n "$1" || return "$LINENO"
  sed \
    -e 's/a/日/g' \
    -e 's/b/月/g' \
    -e 's/c/金/g' \
    -e 's/d/木/g' \
    -e 's/e/水/g' \
    -e 's/f/火/g' \
    -e 's/g/土/g' \
    -e 's/h/竹/g' \
    -e 's/i/戈/g' \
    -e 's/j/十/g' \
    -e 's/k/大/g' \
    -e 's/l/中/g' \
    -e 's/m/一/g' \
    -e 's/n/弓/g' \
    -e 's/o/人/g' \
    -e 's/p/心/g' \
    -e 's/q/手/g' \
    -e 's/r/口/g' \
    -e 's/s/尸/g' \
    -e 's/t/廿/g' \
    -e 's/u/山/g' \
    -e 's/v/女/g' \
    -e 's/w/田/g' \
    -e 's/y/卜/g' \
    -e 's/z/重/g' \
    -e 's/x/難/g' \
  <<<"$1"
}

# | grep --basic-regexp --regexp="^# " --invert-match \
function grep2 {
  test "$#" -eq 1 -a "${#1}" -eq 1 || return "$LINENO"
  grep --fixed-string --regexp="$1" /usr/share/rime-data/cangjie5.dict.yaml \
    | grep --extended-regexp --regexp="^."$'\t'"[a-z]+$" \
  || echo "$1"$'\t'"$1" # Tolerate characters not in dictionary
}

function lookup_str {

  test "$#" -eq 1 -a -n "$1" || return "$LINENO"

  local output=()

  # echo "$1 $((${#1}))"
  for i in $(seq 0 $((${#1}-1))); do
    # echo "B"
    local char="${1:i:1}"
    local raw=""
    local line=""
    local code=""
    raw="$(grep2 "$char")"
    # echo "$raw"
    # exit 0
    test "$(wc -l <<<"$raw")" -ge 1 || return "$LINENO"
    while IFS= read -r line; do
      # echo "... $line ..."
      # echo "C"
      # echo "= $char = $(awk -F $'\t' '{print $1}' <<<"$line") ="
      # echo "> $line"
      test "$char" = "$(awk -F $'\t' '{print $1}' <<<"$line")" || return "$LINENO"
      # echo "D"
      code="$(awk -F $'\t' '{print $2}' <<<"$line")"
      # echo "$char - $(convert "$code") - $code"
      output+=("$char" "$(convert "$code")" "$code")
    done <<< "$raw"
  done

  local cells="${#output[@]}"
  test "$cells" -ge 3 -a "$((cells%3))" -eq 0 || return "$LINENO"
  zenity \
    --list --title="$TITLE" --text="「取消」終了，「確認」下一個" \
    --column="漢字" --column="倉頡碼" --column="鍵" \
    "${output[@]}" \
    >/dev/null \
    || exit 0

  # # shellcheck disable=SC2086
  # zenity \
  #   --list --title="$TITLE" --text="「取消」終了，「確認」下一個" \
  #   --column="漢字" --column="倉頡碼" --column="鍵" \
  #   $output \
  #   >/dev/null \
  #   || exit 0

}

function input {
  test "$#" -eq 0 || return "$LINENO"
  zenity --entry --window-icon=question --title="$TITLE" --text="「取消」放棄，「確認」查詢" # Return 1 on Cancel
}

function main_loop {
  while true; do
    str="$(input)" || exit 0
    test -n "$str" || exit 0
    lookup_str "$str"
    str=""
  done
}

# lookup_str "什麼"
# lookup_str "日.日"
# lookup_str "依字源，「賴」字從「貝」，「剌」聲。「刀」在右旁變形成「刂」，在右上角可寫回「刀」形。取碼字形較正統。"

# main_loop
main_loop &>/dev/null &
