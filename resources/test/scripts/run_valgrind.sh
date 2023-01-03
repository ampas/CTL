#!/bin/bash

set -x
set -u 

# get initial path before changing it
SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

cd ./unittest/IlmCtl/
valgrind -s --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all ./IlmCtlTest
test_01_status=$? 
test_01_label="IlmCtlTest"

cd ../IlmCtlMath/
valgrind -s --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all ./IlmCtlMathTest 
test_02_status=$? 
test_02_label="IlmCtlMathTest"

cd ../IlmImfCtl/
valgrind -s --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all ./IlmImfCtlTest
test_03_status=$? 
test_03_label="IlmImfCtlTest"

cd ../ctlrender
valgrind -s --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all ../../ctlrender/ctlrender -force -ctl ../../../unittest/ctlrender/unity.ctl ../../../unittest/ctlrender/bars_nuke_10_be.dpx out.dpx
test_04_status=$? 
test_04_label="ctlrender-dpx-to-dpx"

valgrind -s --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all ../../ctlrender/ctlrender -force -ctl ../../../unittest/ctlrender/unity.ctl ../../../unittest/ctlrender/bars_nuke_10_be.dpx out.tif
test_05_status=$? 
test_05_label="ctlrender-dpx-to-tif"

valgrind -s --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all ../../ctlrender/ctlrender -force -ctl ../../../unittest/ctlrender/unity.ctl ../../../unittest/ctlrender/bars_photoshop_16_le_interleaved.tif out.tif
test_06_status=$? 
test_06_label="ctlrender-tif-to-tif"

valgrind -s --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all ../../ctlrender/ctlrender -force -ctl ../../../unittest/ctlrender/unity.ctl ../../../unittest/ctlrender/bars_photoshop_16_le_interleaved.tif out.dpx
test_07_status=$? 
test_07_label="ctlrender-tif-to-dpx"

valgrind -s --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all ../../ctlrender/ctlrender -force -ctl ../../../unittest/ctlrender/unity.ctl ../../../unittest/ctlrender/bars_photoshop.exr out.exr
test_08_status=$? 
test_08_label="ctlrender-exr-to-exr"

valgrind -s --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all ../../ctlrender/ctlrender -force -format exr16 -ctl ../../../unittest/ctlrender/unity.ctl ../../../unittest/ctlrender/bars_photoshop.exr out.exr
test_09_status=$? 
test_09_label="ctlrender-exr-to-exr16"

valgrind -s --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all ../../ctlrender/ctlrender -force -format exr32 -ctl ../../../unittest/ctlrender/unity.ctl ../../../unittest/ctlrender/bars_photoshop.exr out.exr
test_10_status=$? 
test_10_label="ctlrender-exr-to-exr32"

# go back to initial path
cd $SCRIPTPATH

# return valgrind exit codes
if [ $test_01_status -eq 0 ] && [ $test_02_status -eq 0 ] && [ $test_03_status -eq 0 ] && [ $test_04_status -eq 0 ] && [ $test_05_status -eq 0 ] && [ $test_06_status -eq 0 ] && [ $test_07_status -eq 0 ] && [ $test_08_status -eq 0 ] && [ $test_09_status -eq 0 ] && [ $test_10_status -eq 0 ]
then
  echo "Success: valgrind detected no errors"
  exit 0
else
  echo "Failure: valgrind detected errors"

  if [ $test_01_status -ne 0 ]
  then 
    echo "$test_01_label: valgrind detected errors" 
  fi

  if [ $test_02_status -ne 0 ]
  then 
    echo "$test_02_label: valgrind detected errors" 
  fi

  if [ $test_03_status -ne 0 ]
  then 
    echo "$test_03_label: valgrind detected errors" 
  fi

  if [ $test_04_status -ne 0 ]
  then 
    echo "$test_04_label: valgrind detected errors" 
  fi

  if [ $test_05_status -ne 0 ]
  then 
    echo "$test_05_label: valgrind detected errors" 
  fi

  if [ $test_06_status -ne 0 ]
  then 
    echo "$test_06_label: valgrind detected errors" 
  fi

  if [ $test_07_status -ne 0 ]
  then 
    echo "$test_07_label: valgrind detected errors" 
  fi

  if [ $test_08_status -ne 0 ]
  then 
    echo "$test_08_label: valgrind detected errors" 
  fi

  if [ $test_09_status -ne 0 ]
  then 
    echo "$test_09_label: valgrind detected errors" 
  fi

  if [ $test_10_status -ne 0 ]
  then 
    echo "$test_10_label: valgrind detected errors" 
  fi

  exit 1
fi
