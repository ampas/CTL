if(NOT DEFINED EXRDPX_BUILD_DIR)
    set(EXRDPX_BUILD_DIR "${CMAKE_BINARY_DIR}/OpenEXR_CTL/exrdpx")
    message("exrdpx build director: ${EXRDPX_BUILD_DIR}")
endif()

if(NOT DEFINED EXRDPX_EXECUTABLE_PATH)
    set(EXRDPX_EXECUTABLE_PATH "${EXRDPX_BUILD_DIR}/exrdpx")
    message("exrdpx test executable: ${EXRDPX_EXECUTABLE_PATH}")
endif()

set(EXRDPX_OUTPUT_FOLDER "${CMAKE_BINARY_DIR}/unittest/exrdpx/output")
message("exrdpx test output folder will be created at: ${EXRDPX_OUTPUT_FOLDER}")
add_custom_target(exrdpx-build-time-make-directory ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${EXRDPX_OUTPUT_FOLDER})

set(TEST_FILES "${PROJECT_SOURCE_DIR}/unittest/exrdpx")

# copy CTL files to build folder
add_custom_target(
  exrdpx-build-time-copy-CTL-files ALL
  COMMAND ${CMAKE_COMMAND} -E copy 
    ${TEST_FILES}/*.ctl 
    "${EXRDPX_BUILD_DIR}")

if(OpenEXR_FOUND)
    # Test EXR->DPX conversions
    add_test(NAME "exrdpx-DPX-to-EXR-noargs" COMMAND ${EXRDPX_EXECUTABLE_PATH} "${TEST_FILES}/marci-512.dpx" "${EXRDPX_OUTPUT_FOLDER}/exrdpx-DPX-to-EXR-noargs.exr" WORKING_DIRECTORY "${EXRDPX_BUILD_DIR}")

    # Test DPX->EXR conversions
    add_test(NAME "exrdpx-EXR-to-DPX-noargs" COMMAND ${EXRDPX_EXECUTABLE_PATH} "${TEST_FILES}/marci-512.exr" "${EXRDPX_OUTPUT_FOLDER}/exrdpx-EXR-to-DPX-noargs.dpx" WORKING_DIRECTORY "${EXRDPX_BUILD_DIR}")
endif()

