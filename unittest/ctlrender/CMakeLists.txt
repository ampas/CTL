

find_program(BASH_PROGRAM bash)
if (BASH_PROGRAM)
  MESSAGE( STATUS "BASH found, able to run ctlrender unit test" )
  add_test(
        NAME ctlrender
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${BASH_PROGRAM} ./test.sh $<TARGET_FILE:ctlrender>
    )
else()
    MESSAGE( STATUS "BASH not found, able to run ctlrender unit test" )
endif (BASH_PROGRAM)



find_package(TIFF)
if ( TARGET TIFF::TIFF )
    set_property(TEST ctlrender PROPERTY ENVIRONMENT "IS_TIFF_FOUND=1")
else()
    message(WARNING "LibTIFF not found, skipping tiff-related ctlrender unit tests")
    set_property(TEST ctlrender PROPERTY ENVIRONMENT "IS_TIFF_FOUND=0")
endif()


