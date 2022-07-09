
enable_testing()

add_custom_target( check COMMAND ${CMAKE_CTEST_COMMAND} )

add_subdirectory( IlmCtl )
add_subdirectory( IlmCtlMath )
add_subdirectory( IlmImfCtl )
# This needs to be able to find aces_container.dylib, and the hints should include
# not just /usr/local/lib, but CMAKE_INSTALL_PREFIX/lib as well. But I don't want
# to take the time today to write FindAcesContainer.
# add_subdirectory( ctlrender )

