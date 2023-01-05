

find_program(BASH_PROGRAM bash)
if (BASH_PROGRAM)
  MESSAGE( STATUS "BASH found, able to run ctlrender unit test" )
  add_test(
        NAME ctlrender
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${BASH_PROGRAM} ./test.sh $<TARGET_FILE:ctlrender>
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

add_test(NAME "ctlrender-ctl-function-name-is-filename" COMMAND ctlrender -force -format exr32 -ctl "${PROJECT_SOURCE_DIR}/unittest/ctlrender/functionname_is_filename.ctl" "${PROJECT_SOURCE_DIR}/unittest/ctlrender/bars_photoshop.exr" out.exr)
