if(NOT DEFINED CTLRENDER_PATH)
    set(CTLRENDER_PATH "${CMAKE_BINARY_DIR}/ctlrender/ctlrender")
endif()

find_package(TIFF)

set(OUTPUT_FOLDER "${CMAKE_BINARY_DIR}/unittest/ctlrender/output")
message("Test output folder will be created at: ${OUTPUT_FOLDER}")
add_custom_target(build-time-make-directory ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_FOLDER})

foreach(J tiff8 tiff16 tiff32 dpx8 dpx10 dpx12 dpx16)
    foreach(I  bars_cinepaint_10.dpx bars_nuke_10_be.dpx bars_nuke_10_le.dpx bars_nuke_12_be.dpx bars_nuke_12_le.dpx bars_nuke_16_be.dpx bars_nuke_16_le.dpx bars_nuke_8_be.dpx bars_nuke_8_le.dpx bars_photoshop_16_be_interleaved.tif bars_photoshop_16_be_planar.tif bars_photoshop_16_le_interleaved.tif bars_photoshop_16_le_planar.tif bars_photoshop_32_be_interleaved.tif bars_photoshop_32_be_planar.tif bars_photoshop_32_le_interleaved.tif bars_photoshop_32_le_planar.tif bars_photoshop_8_be_interleaved.tif bars_photoshop_8_be_planar.tif bars_photoshop_8_le_interleaved.tif bars_photoshop_8_le_planar.tif)
        # Get extension from ${J}
        string(REGEX REPLACE "[0-9]+" "" EXTENSION "${J}")
        # Get filename from ${I}
        string(REGEX REPLACE "\\..+" "" FILE_NAME "${I}")
        # Get file extension from ${I}
        string(REGEX REPLACE ".+\\." "" FILE_NAME_EXTENSION "${I}")
        if (NOT TARGET TIFF::TIFF AND ${EXTENSION} STREQUAL "tiff")
            message("Skipping ${J} because tiff support was not detected")
        elseif(NOT TARGET TIFF::TIFF AND ${FILE_NAME_EXTENSION} STREQUAL "tif")
            message("Skipping ${J} because tiff support was not detected")
        else()
            add_test(NAME "${I}->${J}_unity" COMMAND ${CTLRENDER_PATH} -ctl "${PROJECT_SOURCE_DIR}/unittest/ctlrender/unity.ctl" -format ${J} -force "${PROJECT_SOURCE_DIR}/unittest/ctlrender/${I}" "${OUTPUT_FOLDER}/${FILE_NAME}_${J}.${EXTENSION}")
        endif()
    endforeach(I)

    # Test tiff32 conversions
    if(TARGET TIFF::TIFF)
        add_test(NAME "tiff32->${J}" COMMAND ${CTLRENDER_PATH} -ctl "${PROJECT_SOURCE_DIR}/unittest/ctlrender/unity.ctl" -format ${J} -force "${PROJECT_SOURCE_DIR}/unittest/ctlrender/bars_photoshop_32_be_planar.tif" "${OUTPUT_FOLDER}/bars_tiff32_${J}.${EXTENSION}")
        if(NOT ${J} STREQUAL "tiff32")
            add_test(NAME "${J}->tiff32" COMMAND ${CTLRENDER_PATH} -ctl "${PROJECT_SOURCE_DIR}/unittest/ctlrender/unity.ctl" -format tiff32 -force "${OUTPUT_FOLDER}/bars_tiff32_${J}.${EXTENSION}" "${OUTPUT_FOLDER}/bars_tiff32_${J}_tiff32.tiff")
        endif()
    endif()
endforeach(J)

if(OpenEXR_FOUND)
    # Test EXR->EXR conversions
    add_test(NAME "EXR->EXR" COMMAND ${CTLRENDER_PATH} -ctl "${PROJECT_SOURCE_DIR}/unittest/ctlrender/unity.ctl" -format exr -force "${PROJECT_SOURCE_DIR}/unittest/ctlrender/bars_photoshop.exr" "${OUTPUT_FOLDER}/bars_exr_exr.exr")
    add_test(NAME "EXR->EXR16" COMMAND ${CTLRENDER_PATH} -ctl "${PROJECT_SOURCE_DIR}/unittest/ctlrender/unity.ctl" -format exr16 -force "${PROJECT_SOURCE_DIR}/unittest/ctlrender/bars_photoshop.exr" "${OUTPUT_FOLDER}/bars_exr_exr16.exr")
endif()

#find_program(BASH_PROGRAM bash)
#if (BASH_PROGRAM)
#  MESSAGE( STATUS "BASH found, able to run ctlrender unit test" )
#  add_test(
#        NAME ctlrender
#        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#        COMMAND ${BASH_PROGRAM} ./test.sh ${CTLRENDER_PATH}
#    )
#else()
#    MESSAGE( STATUS "BASH not found, not able to run ctlrender unit test" )
#endif (BASH_PROGRAM)


if ( TARGET TIFF::TIFF AND OpenEXR_FOUND )
    message(STATUS "LibTIFF found, OpenEXR found, including TIF and EXR files in ctlrender unit tests")
#    set_property(TEST ctlrender PROPERTY ENVIRONMENT "IS_TIFF_FOUND=1;IS_OPENEXR_FOUND=1")
elseif( TARGET TIFF::TIFF AND NOT OpenEXR_FOUND )
    message(WARNING "LibTIFF found, OpenEXR not found, including TIF files in ctlrender unit tests")
#    set_property(TEST ctlrender PROPERTY ENVIRONMENT "IS_TIFF_FOUND=1;IS_OPENEXR_FOUND=0")
elseif( NOT TARGET TIFF::TIFF AND OpenEXR_FOUND )
    message(WARNING "LibTIFF not found, OpenEXR found, including EXR files in ctlrender unit tests")
#    set_property(TEST ctlrender PROPERTY ENVIRONMENT "IS_TIFF_FOUND=0;IS_OPENEXR_FOUND=1")
elseif( NOT TARGET TIFF::TIFF AND NOT OpenEXR_FOUND )
    message(WARNING "LibTIFF not found, OpenEXR not found, not including TIF or EXR files in ctlrender unit tests")
#    set_property(TEST ctlrender PROPERTY ENVIRONMENT "IS_TIFF_FOUND=0;IS_OPENEXR_FOUND=0")
endif()

add_test(NAME "ctlrender-ctl-function-name-is-filename" COMMAND ctlrender -force -format exr32 -ctl "${PROJECT_SOURCE_DIR}/unittest/ctlrender/functionname_is_filename.ctl" "${PROJECT_SOURCE_DIR}/unittest/ctlrender/bars_photoshop.exr" out.exr)

add_test(NAME "ctlrender-ctl-function-name-is-incorrect" COMMAND ctlrender -force -format exr32 -ctl "${PROJECT_SOURCE_DIR}/unittest/ctlrender/incorrect_function_name.ctl" "${PROJECT_SOURCE_DIR}/unittest/ctlrender/bars_photoshop.exr" out.exr)
set_tests_properties("ctlrender-ctl-function-name-is-incorrect" PROPERTIES WILL_FAIL TRUE)
