#!/bin/bash

echo
echo
echo

{
  echo
  echo "stderr $(date)"
  echo
} 1>&2

{
  echo
  echo "stdout $(date)"
  echo
  env
  echo
  echo "-------"
  cat
  echo "-------"
  echo
}

echo
echo
echo
