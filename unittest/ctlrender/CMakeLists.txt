

add_test(
        NAME ctlrender
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ./test.sh $<TARGET_FILE:ctlrender>
    )

find_package(TIFF)
if ( TARGET TIFF::TIFF )
    set_property(TEST ctlrender PROPERTY ENVIRONMENT "IS_TIFF_FOUND=1")
else()
    message(WARNING "LibTIFF not found, skipping tiff-related ctlrender unit tests")
    set_property(TEST ctlrender PROPERTY ENVIRONMENT "IS_TIFF_FOUND=0")
endif()

#add_dependencies(check ctlrender)
