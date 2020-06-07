#!/bin/bash

# 日月金木水火土 竹戈十大中一弓 人心手口 尸廿山女田卜
# abcdefg hijklmn opqr stuvwy

# abcdefghijklmnopqrstuvwy
# 日月金木水火土竹戈十大中一弓人心手口尸廿山女田卜

[ "$#" -eq 1 ] || exit 1

for i in $(seq 0 $((${#1}-1))); do
  grep -Fe "${1:i:1}" /usr/share/rime-data/cangjie5.dict.yaml \
    | grep -ve '^#' \
    | sed \
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
      \
      -e 's/z/重/g' \
      -e 's/x/難/g'
done
