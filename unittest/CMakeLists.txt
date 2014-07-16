
enable_testing()

add_custom_target( check COMMAND ${CMAKE_CTEST_COMMAND} )

add_subdirectory( IlmCtl )
add_subdirectory( IlmCtlMath )
add_subdirectory( IlmImfCtl )
add_subdirectory( ctlrender )

