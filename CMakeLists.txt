cmake_minimum_required(VERSION 2.8)
project( CTL )

set( CTL_MAJOR_VERSION 1 )
set( CTL_MINOR_VERSION 5 )
set( CTL_PATCH_VERSION 0 )
set( CTL_VERSION ${CTL_MAJOR_VERSION}.${CTL_MINOR_VERSION}.${CTL_PATCH_VERSION} )

## Make install directories overrideable
set( INSTALL_LIB_DIR lib CACHE PATH "Install directory for libraries" )
set( INSTALL_BIN_DIR bin CACHE PATH "Install directory for executable binaries" )
set( INSTALL_INCLUDE_DIR include CACHE PATH "Install directory for public header files" )
set( INSTALL_DOC_DIR doc CACHE PATH "Install directory for documentation" )
if( WIN32 AND NOT CYGWIN )
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/CMake/CTL)
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Install directory for project CMake files" )
SET( CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib" )

## convert install paths to absolute
foreach( p LIB BIN INCLUDE CMAKE DOC)
  set( var INSTALL_${p}_DIR )
  if( NOT IS_ABSOLUTE "${${var}}" )
    set( ${var} "${CMAKE_INSTALL_PREFIX}/${${var}}" )
  endif()
endforeach()

option(ENABLE_SHARED "Enable Shared Libraries" ON)

if ( ENABLE_SHARED )
set( DO_SHARED SHARED )
else()
set( DO_SHARED STATIC )
endif()

include( configure.cmake )

# generated config files end up in binary dir so to find them, need
# to add to include path
include_directories( "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}" )
# implement find library stuff...

###
### Everyone (well, except for DPX) uses IlmBase, so
### make that a global setting
###
if ( IlmBase_FOUND )
include_directories( ${IlmBase_INCLUDE_DIRS} )
link_directories( ${IlmBase_LIBRARY_DIRS} )
endif()

add_definitions( -DPACKAGE="CTL" -DVERSION="${CTL_VERSION}" )
add_subdirectory(doc)
add_subdirectory(lib)
add_subdirectory(ctlrender)
add_subdirectory(OpenEXR_CTL)
add_subdirectory(Nuke_CTL)
add_subdirectory(unittest EXCLUDE_FROM_ALL)

# Add all targets to the build-tree export set
export( TARGETS IlmCtl IlmCtlMath IlmCtlSimd FILE "${PROJECT_BINARY_DIR}/CTLLibraryDepends.cmake" )
export(PACKAGE CTL)
 
# Create a CTLBuildTreeSettings.cmake file for the use from the build tree
file(RELATIVE_PATH CONF_REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}" "${INSTALL_INCLUDE_DIR}")
configure_file(config/CTLBuildTreeSettings.cmake.in "${PROJECT_BINARY_DIR}/CTLBuildTreeSettings.cmake" @ONLY)
configure_file(config/CTLConfig.cmake.in "${PROJECT_BINARY_DIR}/CTLConfig.cmake" @ONLY)
configure_file(config/CTLConfigVersion.cmake.in "${PROJECT_BINARY_DIR}/CTLConfigVersion.cmake" @ONLY)

if ( PKG_CONFIG_FOUND )
configure_file(config/CTL.pc.in "${PROJECT_BINARY_DIR}/CTL.pc" @ONLY)
install( FILES "${PROJECT_BINARY_DIR}/CTL.pc" DESTINATION lib/pkgconfig COMPONENT dev )
endif()

install( FILES
  "${PROJECT_BINARY_DIR}/CTLConfig.cmake"
  "${PROJECT_BINARY_DIR}/CTLConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)
install(FILES "${PROJECT_BINARY_DIR}/CTLLibraryDepends.cmake" DESTINATION
  "${INSTALL_CMAKE_DIR}" COMPONENT dev)

