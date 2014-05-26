#! /bin/bash
CTLRENDER=$1

mkdir output

for J in tiff8 tiff16 tiff32 dpx8 dpx10 dpx12 dpx16; do
	for I in bars_cinepaint_10.dpx bars_nuke_10_be.dpx bars_nuke_10_le.dpx bars_nuke_12_be.dpx bars_nuke_12_le.dpx bars_nuke_16_be.dpx bars_nuke_16_le.dpx bars_nuke_8_be.dpx bars_nuke_8_le.dpx bars_photoshop_16_be_interleaved.tif bars_photoshop_16_be_planar.tif bars_photoshop_16_le_interleaved.tif bars_photoshop_16_le_planar.tif bars_photoshop_32_be_interleaved.tif bars_photoshop_32_be_planar.tif bars_photoshop_32_le_interleaved.tif bars_photoshop_32_le_planar.tif bars_photoshop_8_be_interleaved.tif bars_photoshop_8_be_planar.tif bars_photoshop_8_le_interleaved.tif bars_photoshop_8_le_planar.tif ; do
		name=`echo $I | sed -e 's/\..*//'`
		ext=`echo $J | sed -e 's/[0-9]//g'`
		echo ${I} '->' ${J}
		$CTLRENDER -ctl unity.ctl -format ${J} -force ${I} output/${name}_${J}.${ext}
	done
	echo ${J} unity test
	$CTLRENDER -ctl unity.ctl -format ${J} -force bars_photoshop_32_be_planar.tif output/bars_tiff32_${J}.${ext}
	$CTLRENDER -ctl unity.ctl -format tiff32 -force output/bars_tiff32_${J}.${ext} output/bars_tiff32_${J}_tiff32.tiff
done

