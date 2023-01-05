if(!CTLRENDER_PATH)
    set(CTLRENDER_PATH, $<TARGET_FILE:ctlrender>)
endif()

foreach(J tiff8 tiff16 tiff32 dpx8 dpx10 dpx12 dpx16)
    foreach(I  bars_cinepaint_10.dpx bars_nuke_10_be.dpx bars_nuke_10_le.dpx bars_nuke_12_be.dpx bars_nuke_12_le.dpx bars_nuke_16_be.dpx bars_nuke_16_le.dpx bars_nuke_8_be.dpx bars_nuke_8_le.dpx bars_photoshop_16_be_interleaved.tif bars_photoshop_16_be_planar.tif bars_photoshop_16_le_interleaved.tif bars_photoshop_16_le_planar.tif bars_photoshop_32_be_interleaved.tif bars_photoshop_32_be_planar.tif bars_photoshop_32_le_interleaved.tif bars_photoshop_32_le_planar.tif bars_photoshop_8_be_interleaved.tif bars_photoshop_8_be_planar.tif bars_photoshop_8_le_interleaved.tif bars_photoshop_8_le_planar.tif)
        string(REGEX REPLACE "[0-9]+" "" EXTENSION "${J}")
        string(REGEX REPLACE "\\..+" "" FILE_NAME "${I}")
        add_test(NAME "${I}->${J}_unity" COMMAND ${CTLRENDER_PATH} -ctl "${PROJECT_SOURCE_DIR}/unittest/ctlrender/unity.ctl" -format ${J} -force ${PROJECT_SOURCE_DIR}/unittest/ctlrender/${I} "${PROJECT_SOURCE_DIR}/unittest/ctlrender/output/${FILE_NAME}_${J}.${EXTENSION}")
    endforeach(I)
endforeach(J)

find_program(BASH_PROGRAM bash)
if (BASH_PROGRAM)
  MESSAGE( STATUS "BASH found, able to run ctlrender unit test" )
  add_test(
        NAME ctlrender
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${BASH_PROGRAM} ./test.sh ${CTLRENDER_PATH}
    )
else()
    MESSAGE( STATUS "BASH not found, not able to run ctlrender unit test" )
endif (BASH_PROGRAM)



find_package(TIFF)
if ( TARGET TIFF::TIFF AND OpenEXR_FOUND )
    message(WARNING "LibTIFF found, OpenEXR found, including TIF and EXR files in ctlrender unit tests")
    set_property(TEST ctlrender PROPERTY ENVIRONMENT "IS_TIFF_FOUND=1;IS_OPENEXR_FOUND=1")
elseif( TARGET TIFF::TIFF AND NOT OpenEXR_FOUND )
    message(WARNING "LibTIFF found, OpenEXR not found, including TIF files in ctlrender unit tests")
    set_property(TEST ctlrender PROPERTY ENVIRONMENT "IS_TIFF_FOUND=1;IS_OPENEXR_FOUND=0")
elseif( NOT TARGET TIFF::TIFF AND OpenEXR_FOUND )
    message(WARNING "LibTIFF not found, OpenEXR found, including EXR files in ctlrender unit tests")
    set_property(TEST ctlrender PROPERTY ENVIRONMENT "IS_TIFF_FOUND=0;IS_OPENEXR_FOUND=1")
elseif( NOT TARGET TIFF::TIFF AND NOT OpenEXR_FOUND )
    message(WARNING "LibTIFF not found, OpenEXR not found, not including TIF or EXR files in ctlrender unit tests")
    set_property(TEST ctlrender PROPERTY ENVIRONMENT "IS_TIFF_FOUND=0;IS_OPENEXR_FOUND=0")
endif()


