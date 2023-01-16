

if ( NOT OpenEXR_FOUND )
    message(WARNING "OpenEXR not found, Skipping OpenEXR_CTL")
    return()
endif()

add_subdirectory( CtlModules )
add_subdirectory( exrdpx )
add_subdirectory( exr_ctl_exr )

add_custom_target( OpenEXR_CTL DEPENDS CtlModules exrdpx exr_ctl_exr )


# TODO CMake config

