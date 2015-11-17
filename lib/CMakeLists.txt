add_subdirectory( IlmCtl )
add_subdirectory( IlmCtlMath )
add_subdirectory( IlmCtlSimd )

if ( OpenEXR_FOUND )
add_subdirectory( IlmImfCtl )
endif()

add_subdirectory( dpx )
add_subdirectory( adx )


add_custom_target( CTL DEPENDS IlmCtl IlmCtlMath IlmCtlSimd )

### TODO: Add install, pkgconfig stuff
