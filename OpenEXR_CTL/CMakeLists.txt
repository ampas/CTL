

if ( NOT OpenEXR_FOUND )
message(WARNING "OpenEXR not found, Skipping OpenEXR_CTL")
return()
endif()

add_subdirectory( CtlModules )
add_subdirectory( exrdpx )
add_subdirectory( exr_ctl_exr )

add_custom_target( OpenEXR_CTL DEPENDS CtlModules exrdpx exr_ctl_exr )

if ( PKG_CONFIG_FOUND )
configure_file(../config/OpenEXR_CTL.pc.in "${PROJECT_BINARY_DIR}/OpenEXR_CTL.pc" @ONLY)
install( FILES "${PROJECT_BINARY_DIR}/OpenEXR_CTL.pc" DESTINATION lib/pkgconfig COMPONENT dev )
endif()

# TODO CMake config

