if(NOT DEFINED EXR_CTL_EXR_BUILD_DIR)
    set(EXR_CTL_EXR_BUILD_DIR "${CMAKE_BINARY_DIR}/OpenEXR_CTL/exr_ctl_exr")
    message("exr_ctl_exr build director: ${EXR_CTL_EXR_BUILD_DIR}")
endif()

if(NOT DEFINED EXR_CTL_EXR_EXECUTABLE_PATH)
    set(EXR_CTL_EXR_EXECUTABLE_PATH "${EXR_CTL_EXR_BUILD_DIR}/exr_ctl_exr")
    message("exr_ctl_exr test executable: ${EXR_CTL_EXR_EXECUTABLE_PATH}")
endif()

set(EXR_CTL_EXR_OUTPUT_FOLDER "${CMAKE_BINARY_DIR}/unittest/exr_ctl_exr/output")
message("exr_ctl_exr test output folder will be created at: ${EXR_CTL_EXR_OUTPUT_FOLDER}")
add_custom_target(exr-ctl-exr-build-time-make-directory ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${EXR_CTL_EXR_OUTPUT_FOLDER})

set(TEST_FILES "${PROJECT_SOURCE_DIR}/unittest/exr_ctl_exr")

# copy CTL files to build folder
add_custom_target(
    exr-ctl-exr-build-time-copy-CTL-file1 ALL
  COMMAND ${CMAKE_COMMAND} -E copy 
    ${TEST_FILES}/change_saturation.ctl 
    "${EXR_CTL_EXR_BUILD_DIR}")

if(OpenEXR_FOUND)

    add_test(NAME "exr-ctl-exr-noargs" COMMAND ${EXR_CTL_EXR_EXECUTABLE_PATH} -C change_saturation "${TEST_FILES}/marci-512.exr" "${EXR_CTL_EXR_OUTPUT_FOLDER}/exr-ctl-exr-noargs.exr" WORKING_DIRECTORY "${EXR_CTL_EXR_BUILD_DIR}")
    # set CTL_MODULE_PATH environment variable before running exr_ctl_exr
    #set_property(TEST "exr-ctl-exr-noargs" PROPERTY ENVIRONMENT_MODIFICATION "CTL_MODULE_PATH=./")

endif()
