if(NOT DEFINED CTLRENDER_PATH)
    set(CTLRENDER_PATH "${CMAKE_BINARY_DIR}/ctlrender/ctlrender")
endif()

find_package(TIFF)
if ( TARGET TIFF::TIFF AND OpenEXR_FOUND )
    message(STATUS "LibTIFF found, OpenEXR found, including TIF and EXR files in ctlrender unit tests")
elseif( TARGET TIFF::TIFF AND NOT OpenEXR_FOUND )
    message(WARNING "LibTIFF found, OpenEXR not found, including TIF files in ctlrender unit tests")
elseif( NOT TARGET TIFF::TIFF AND OpenEXR_FOUND )
    message(WARNING "LibTIFF not found, OpenEXR found, including EXR files in ctlrender unit tests")
elseif( NOT TARGET TIFF::TIFF AND NOT OpenEXR_FOUND )
    message(WARNING "LibTIFF not found, OpenEXR not found, not including TIF or EXR files in ctlrender unit tests")
endif()

set(OUTPUT_FOLDER "${CMAKE_BINARY_DIR}/unittest/ctlrender/output")
message("Test output folder will be created at: ${OUTPUT_FOLDER}")
add_custom_target(build-time-make-directory ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_FOLDER})

set(TEST_FILES "${PROJECT_SOURCE_DIR}/unittest/ctlrender")

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
            add_test(NAME "${I}->${J}_unity" COMMAND ${CTLRENDER_PATH} -ctl "${TEST_FILES}/unity.ctl" -format ${J} -force "${TEST_FILES}/${I}" "${OUTPUT_FOLDER}/${FILE_NAME}_${J}.${EXTENSION}")
        endif()
    endforeach(I)

    # Test tiff32 conversions
    if(TARGET TIFF::TIFF)
        add_test(NAME "tiff32->${J}" COMMAND ${CTLRENDER_PATH} -ctl "${TEST_FILES}/unity.ctl" -format ${J} -force "${TEST_FILES}/bars_photoshop_32_be_planar.tif" "${OUTPUT_FOLDER}/bars_tiff32_${J}.${EXTENSION}")
        if(NOT ${J} STREQUAL "tiff32")
            add_test(NAME "${J}->tiff32" COMMAND ${CTLRENDER_PATH} -ctl "${TEST_FILES}/unity.ctl" -format tiff32 -force "${OUTPUT_FOLDER}/bars_tiff32_${J}.${EXTENSION}" "${OUTPUT_FOLDER}/bars_tiff32_${J}_tiff32.tiff")
        endif()
    endif()
endforeach(J)

if(OpenEXR_FOUND)
    # Test EXR->EXR conversions
    add_test(NAME "EXR->EXR" COMMAND ${CTLRENDER_PATH} -ctl "${TEST_FILES}/unity.ctl" -format exr -force "${TEST_FILES}/bars_photoshop.exr" "${OUTPUT_FOLDER}/bars_exr_exr.exr")
    add_test(NAME "EXR->EXR16" COMMAND ${CTLRENDER_PATH} -ctl "${TEST_FILES}/unity.ctl" -format exr16 -force "${TEST_FILES}/bars_photoshop.exr" "${OUTPUT_FOLDER}/bars_exr_exr16.exr")
    add_test(NAME "EXR->EXR32" COMMAND ${CTLRENDER_PATH} -ctl "${TEST_FILES}/unity.ctl" -format exr32 -force "${TEST_FILES}/bars_photoshop.exr" "${OUTPUT_FOLDER}/bars_exr_exr32.exr")

    # RGB Tests
    add_test(NAME "EXR16->EXR16" COMMAND ${CTLRENDER_PATH} -ctl "${TEST_FILES}/unity.ctl" -format exr16 -force "${TEST_FILES}/colorbars_nuke_rgb_exr16.exr" "${OUTPUT_FOLDER}/bars_rgb_exr16_to_exr16.exr")
    add_test(NAME "EXR16->EXR32" COMMAND ${CTLRENDER_PATH} -ctl "${TEST_FILES}/unity.ctl" -format exr32 -force "${TEST_FILES}/colorbars_nuke_rgb_exr16.exr" "${OUTPUT_FOLDER}/bars_rgb_exr16_to_exr32.exr")
    add_test(NAME "EXR32->EXR16" COMMAND ${CTLRENDER_PATH} -ctl "${TEST_FILES}/unity.ctl" -format exr16 -force "${TEST_FILES}/colorbars_nuke_rgb_exr32.exr" "${OUTPUT_FOLDER}/bars_rgb_exr32_to_exr16.exr")
    add_test(NAME "EXR32->EXR32" COMMAND ${CTLRENDER_PATH} -ctl "${TEST_FILES}/unity.ctl" -format exr32 -force "${TEST_FILES}/colorbars_nuke_rgb_exr32.exr" "${OUTPUT_FOLDER}/bars_rgb_exr32_to_exr32.exr")

    # RGBA Tests
    add_test(NAME "EXR_RGBA16->EXR_RGBA_16" COMMAND ${CTLRENDER_PATH} -ctl "${TEST_FILES}/unity_with_alpha.ctl" -format exr16 -force "${TEST_FILES}/colorbars_nuke_rgba_exr16.exr" "${OUTPUT_FOLDER}/bars_rgba_exr16_to_exr16.exr")
    add_test(NAME "EXR_RGBA16->EXR_RGBA_32" COMMAND ${CTLRENDER_PATH} -ctl "${TEST_FILES}/unity_with_alpha.ctl" -format exr32 -force "${TEST_FILES}/colorbars_nuke_rgba_exr16.exr" "${OUTPUT_FOLDER}/bars_rgba_exr16_to_exr32.exr")
    add_test(NAME "EXR_RGBA32->EXR_RGBA_16" COMMAND ${CTLRENDER_PATH} -ctl "${TEST_FILES}/unity_with_alpha.ctl" -format exr16 -force "${TEST_FILES}/colorbars_nuke_rgba_exr32.exr" "${OUTPUT_FOLDER}/bars_rgba_exr32_to_exr16.exr")
    add_test(NAME "EXR_RGBA32->EXR_RGBA_32" COMMAND ${CTLRENDER_PATH} -ctl "${TEST_FILES}/unity_with_alpha.ctl" -format exr32 -force "${TEST_FILES}/colorbars_nuke_rgba_exr32.exr" "${OUTPUT_FOLDER}/bars_rgba_exr32_to_exr32.exr")
endif()

# test TIFF32 to EXR and EXR to TIFF32 support
if (OpenEXR_FOUND AND TARGET TIFF::TIFF)
    foreach(J exr exr16 exr32 tiff32)
        foreach(I bars_photoshop_32_be_interleaved.tif bars_photoshop_32_be_planar.tif bars_photoshop_32_le_interleaved.tif bars_photoshop_32_le_planar.tif bars_photoshop.exr)
             # Get extension from ${J}
            string(REGEX REPLACE "[0-9]+" "" EXTENSION "${J}")
            # Get filename from ${I}
            string(REGEX REPLACE "\\..+" "" FILE_NAME "${I}")
            # Get file extension from ${I}
            string(REGEX REPLACE ".+\\." "" FILE_NAME_EXTENSION "${I}")
            if(${J} MATCHES "exr.*" AND FILE_NAME_EXTENSION STREQUAL "exr")
                continue()
            endif()
            if(${J} MATCHES "tif.*" AND FILE_NAME_EXTENSION STREQUAL "tif")
                continue()
            endif()
            add_test(NAME "EXR<->TIF_${I}->${J}" COMMAND ${CTLRENDER_PATH} -ctl "${TEST_FILES}/unity.ctl" -format ${J} -force "${TEST_FILES}/${I}" "${OUTPUT_FOLDER}/${FILE_NAME}_${J}_exr_tif_conversion_test.${EXTENSION}")
        endforeach()
    endforeach()
endif()

add_test(NAME "ctlrender-ctl-function-name-is-filename" COMMAND ctlrender -force -format exr32 -ctl "${TEST_FILES}/functionname_is_filename.ctl" "${TEST_FILES}/bars_photoshop.exr" out.exr)

add_test(NAME "ctlrender-ctl-function-name-is-incorrect" COMMAND ctlrender -force -format exr32 -ctl "${TEST_FILES}/incorrect_function_name.ctl" "${TEST_FILES}/bars_photoshop.exr" out.exr)
set_tests_properties("ctlrender-ctl-function-name-is-incorrect" PROPERTIES WILL_FAIL TRUE)

# Test for invalid input file names
add_test(NAME "ctlrender-invalid-input-filename-dpx" COMMAND ctlrender -force -format dpx -ctl "${TEST_FILES}/unity.ctl" "${TEST_FILES}/invalid_filename.dpx" "${TEST_FILES}/invalid_filename_out.dpx")
set_tests_properties("ctlrender-invalid-input-filename-dpx" PROPERTIES WILL_FAIL TRUE)
if(TARGET TIFF::TIFF)
    add_test(NAME "ctlrender-invalid-input-filename-tiff" COMMAND ctlrender -force -format tiff -ctl "${TEST_FILES}/unity.ctl" "${TEST_FILES}/invalid_filename.tif" "${TEST_FILES}/invalid_filename_out.tif")
    set_tests_properties("ctlrender-invalid-input-filename-tiff" PROPERTIES WILL_FAIL TRUE)
endif()
if(OpenEXR_FOUND)
    add_test(NAME "ctlrender-invalid-input-filename-exr" COMMAND ctlrender -force -format exr -ctl "${TEST_FILES}/unity.ctl" "${TEST_FILES}/invalid_filename.exr" "${TEST_FILES}/invalid_filename_out.exr")
    set_tests_properties("ctlrender-invalid-input-filename-exr" PROPERTIES WILL_FAIL TRUE)
endif()

# Test for invalid output file names
add_test(NAME "ctlrender-invalid-output-filename-dpx" COMMAND ctlrender -force -format dpx -ctl "${TEST_FILES}/unity.ctl" "${TEST_FILES}/bars_nuke_8_be.dpx" "${TEST_FILES}/missing_folder/invalid_filename_out.dpx")
set_tests_properties("ctlrender-invalid-output-filename-dpx" PROPERTIES WILL_FAIL TRUE)
if(TARGET TIFF::TIFF)
    add_test(NAME "ctlrender-invalid-output-filename-tiff" COMMAND ctlrender -force -format tiff -ctl "${TEST_FILES}/unity.ctl" "${TEST_FILES}/bars_photoshop_8_be_interleaved.tif" "${TEST_FILES}/missing_folder/invalid_filename_out.tif")
    set_tests_properties("ctlrender-invalid-output-filename-tiff" PROPERTIES WILL_FAIL TRUE)
endif()
if(OpenEXR_FOUND)
    add_test(NAME "ctlrender-invalid-output-filename-exr" COMMAND ctlrender -force -format exr -ctl "${TEST_FILES}/unity.ctl" "${TEST_FILES}/colorbars_nuke_rgb_exr16.exr" "${TEST_FILES}/missing_folder/invalid_filename_out.exr")
    set_tests_properties("ctlrender-invalid-output-filename-exr" PROPERTIES WILL_FAIL TRUE)
endif()
