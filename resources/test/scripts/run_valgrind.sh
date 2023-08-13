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

valgrind -s --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all ../../ctlrender/ctlrender -force -format exr16 -ctl ../../../unittest/ctlrender/unity.ctl ../../../unittest/ctlrender/colorbars_nuke_rgb_exr16.exr out.exr
test_11_status=$? 
test_11_label="ctlrender-rgb-exr16-to-exr16"

valgrind -s --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all ../../ctlrender/ctlrender -force -format exr32 -ctl ../../../unittest/ctlrender/unity.ctl ../../../unittest/ctlrender/colorbars_nuke_rgb_exr16.exr out.exr
test_12_status=$? 
test_12_label="ctlrender-rgb-exr16-to-exr32"

valgrind -s --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all ../../ctlrender/ctlrender -force -format exr16 -ctl ../../../unittest/ctlrender/unity.ctl ../../../unittest/ctlrender/colorbars_nuke_rgb_exr32.exr out.exr
test_13_status=$? 
test_13_label="ctlrender-rgb-exr32-to-exr16"

valgrind -s --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all ../../ctlrender/ctlrender -force -format exr32 -ctl ../../../unittest/ctlrender/unity.ctl ../../../unittest/ctlrender/colorbars_nuke_rgb_exr32.exr out.exr
test_14_status=$? 
test_14_label="ctlrender-rgb-exr32-to-exr32"

valgrind -s --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all ../../ctlrender/ctlrender -force -format exr16 -ctl ../../../unittest/ctlrender/unity_with_alpha.ctl ../../../unittest/ctlrender/colorbars_nuke_rgba_exr16.exr out.exr
test_15_status=$? 
test_15_label="ctlrender-rgba-exr16-to-exr16"

valgrind -s --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all ../../ctlrender/ctlrender -force -format exr32 -ctl ../../../unittest/ctlrender/unity_with_alpha.ctl ../../../unittest/ctlrender/colorbars_nuke_rgba_exr16.exr out.exr
test_16_status=$? 
test_16_label="ctlrender-rgba-exr16-to-exr32"

valgrind -s --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all ../../ctlrender/ctlrender -force -format exr16 -ctl ../../../unittest/ctlrender/unity_with_alpha.ctl ../../../unittest/ctlrender/colorbars_nuke_rgba_exr32.exr out.exr
test_17_status=$? 
test_17_label="ctlrender-rgba-exr32-to-exr16"

valgrind -s --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all ../../ctlrender/ctlrender -force -format exr32 -ctl ../../../unittest/ctlrender/unity_with_alpha.ctl ../../../unittest/ctlrender/colorbars_nuke_rgba_exr32.exr out.exr
test_18_status=$? 
test_18_label="ctlrender-rgba-exr32-to-exr32"

cd ../exrdpx
cp ../../OpenEXR_CTL/exrdpx/*.ctl .
valgrind -s --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all ../../OpenEXR_CTL/exrdpx/exrdpx ../../../unittest/exrdpx/marci-512.dpx ./output/marci-512.dpx.exr
test_19_status=$? 
test_19_label="exrdpx-dpx-to-exr"

valgrind -s --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all ../../OpenEXR_CTL/exrdpx/exrdpx ../../../unittest/exrdpx/marci-512.exr ./output/marci-512.exr.dpx
test_20_status=$? 
test_20_label="exrdpx-exr-to-dpx"

cd ../exr_ctl_exr
cp ../../OpenEXR_CTL/exr_ctl_exr/*.ctl .
valgrind -s --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all ../../OpenEXR_CTL/exr_ctl_exr/exr_ctl_exr -C change_saturation ../../../unittest/exr_ctl_exr/marci-512.exr ./output/exr_ctl_exr-no-args.exr
test_21_status=$? 
test_21_label="exr_ctl_exr"

valgrind -s --error-exitcode=1 --leak-check=full --track-origins=yes --show-leak-kinds=all ../../OpenEXR_CTL/exr_ctl_exr/exr_ctl_exr -C change_saturation -float sScale 1.2 ../../../unittest/exr_ctl_exr/marci-512.exr ./output/exr_ctl_exr-sScale-1.2.exr
test_22_status=$? 
test_22_label="exr_ctl_exr"

# go back to initial path
cd $SCRIPTPATH

# return valgrind exit codes
if [ $test_01_status -eq 0 ] && [ $test_02_status -eq 0 ] && [ $test_03_status -eq 0 ] && [ $test_04_status -eq 0 ] && [ $test_05_status -eq 0 ] && [ $test_06_status -eq 0 ] && [ $test_07_status -eq 0 ] && [ $test_08_status -eq 0 ] && [ $test_09_status -eq 0 ] && [ $test_10_status -eq 0 ] && [ $test_11_status -eq 0 ] && [ $test_12_status -eq 0 ] && [ $test_13_status -eq 0 ] && [ $test_14_status -eq 0 ] && [ $test_15_status -eq 0 ] && [ $test_16_status -eq 0 ] && [ $test_17_status -eq 0 ] && [ $test_18_status -eq 0 ] && [ $test_19_status -eq 0 ] && [ $test_20_status -eq 0 ] && [ $test_21_status -eq 0 ] && [ $test_22_status -eq 0 ]
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

  if [ $test_11_status -ne 0 ] 
  then 
    echo "$test_11_label: valgrind detected errors" 
  fi

  if [ $test_12_status -ne 0 ] 
  then 
    echo "$test_12_label: valgrind detected errors" 
  fi

  if [ $test_13_status -ne 0 ] 
  then 
    echo "$test_13_label: valgrind detected errors" 
  fi

  if [ $test_14_status -ne 0 ] 
  then 
    echo "$test_14_label: valgrind detected errors" 
  fi

  if [ $test_15_status -ne 0 ] 
  then 
    echo "$test_15_label: valgrind detected errors" 
  fi

  if [ $test_16_status -ne 0 ] 
  then 
    echo "$test_16_label: valgrind detected errors" 
  fi

  if [ $test_17_status -ne 0 ] 
  then 
    echo "$test_17_label: valgrind detected errors" 
  fi

  if [ $test_18_status -ne 0 ] 
  then 
    echo "$test_18_label: valgrind detected errors" 
  fi

  if [ $test_19_status -ne 0 ] 
  then 
    echo "$test_19_label: valgrind detected errors" 
  fi

  if [ $test_20_status -ne 0 ] 
  then 
    echo "$test_20_label: valgrind detected errors" 
  fi

  if [ $test_21_status -ne 0 ] 
  then 
    echo "$test_21_label: valgrind detected errors" 
  fi

  if [ $test_22_status -ne 0 ] 
  then 
    echo "$test_22_label: valgrind detected errors" 
  fi

  exit 1
fi
