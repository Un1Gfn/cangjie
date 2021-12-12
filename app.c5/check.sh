#!/bin/bash

{

  grep -v -E -e '^.+'$'\t''[a-z]+$'

  if [ $? -ne 1 ]; then
    grep -v -E -e '^.+'$'\t''[a-z]+$' | cat -ET
    exit 1
  fi

}; exit 0
