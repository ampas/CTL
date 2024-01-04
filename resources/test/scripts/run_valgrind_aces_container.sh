#!/bin/bash

set -x
set -u 

# get initial path before changing it
SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

cd ./unittest/ctlrender

valgrind -s --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all ../../ctlrender/ctlrender -force -format aces -ctl ../../../unittest/ctlrender/unity.ctl ../../../unittest/ctlrender/colorbars_nuke_rgb_exr16.exr out.aces
test_18b_status=$? 
test_18b_label="ctlrender-rgb-exr16-to-aces"

# go back to initial path
cd $SCRIPTPATH

# return valgrind exit codes
if [ $test_18b_status -eq 0 ]
then
  echo "Success: valgrind detected no errors"
  exit 0
else
  echo "Failure: valgrind detected errors"

  if [ $test_18b_status -ne 0 ] 
  then 
    echo "$test_18b_label: valgrind detected errors" 
  fi

  exit 1
fi
