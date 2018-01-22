cmake_minimum_required(VERSION 2.8)
project( CTL )

INCLUDE ( GNUInstallDirs )

set( CTL_MAJOR_VERSION 1 )
set( CTL_MINOR_VERSION 5 )
set( CTL_PATCH_VERSION 2 )
set( CTL_VERSION ${CTL_MAJOR_VERSION}.${CTL_MINOR_VERSION}.${CTL_PATCH_VERSION} )

set(INSTALL_CMAKE_DIR ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/CTL CACHE PATH "Install directory for project CMake files" )

# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
#SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
# the RPATH to be used when installing, but only if it's not a system directory
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_FULL_LIBDIR}" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
  SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}")
ENDIF("${isSystemDir}" STREQUAL "-1")

option(ENABLE_SHARED "Enable Shared Libraries" ON)

if ( ENABLE_SHARED )
set( DO_SHARED SHARED )
else()
set( DO_SHARED STATIC )
endif()

# Find packages and set up global settings
include( configure.cmake )

# generated config files end up in binary dir so to find them, need
# to add to include path
include_directories( "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}" )
# implement find library stuff...

add_definitions( -DPACKAGE="CTL" -DVERSION="${CTL_VERSION}" )
add_subdirectory(doc)
add_subdirectory(lib)
add_subdirectory(ctlrender)
add_subdirectory(OpenEXR_CTL)
add_subdirectory(unittest EXCLUDE_FROM_ALL)

# Add all targets to the build-tree export set
export( TARGETS IlmCtl IlmCtlMath IlmCtlSimd FILE "${PROJECT_BINARY_DIR}/CTLLibraryDepends.cmake" )
export(PACKAGE CTL)
 
# Create a CTLBuildTreeSettings.cmake file for the use from the build tree
file(RELATIVE_PATH CONF_REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}" "${CMAKE_INSTALL_FULL_INCLUDEDIR}")
configure_file(config/CTLBuildTreeSettings.cmake.in "${PROJECT_BINARY_DIR}/CTLBuildTreeSettings.cmake" @ONLY)
configure_file(config/CTLConfig.cmake.in "${PROJECT_BINARY_DIR}/CTLConfig.cmake" @ONLY)
configure_file(config/CTLConfigVersion.cmake.in "${PROJECT_BINARY_DIR}/CTLConfigVersion.cmake" @ONLY)

if ( PKG_CONFIG_FOUND )
configure_file(config/CTL.pc.in "${PROJECT_BINARY_DIR}/CTL.pc" @ONLY)
install( FILES "${PROJECT_BINARY_DIR}/CTL.pc" DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig COMPONENT dev )
endif()

install( FILES
  "${PROJECT_BINARY_DIR}/CTLConfig.cmake"
  "${PROJECT_BINARY_DIR}/CTLConfigVersion.cmake"
  "${PROJECT_BINARY_DIR}/CTLLibraryDepends.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

