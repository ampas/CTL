#
# A simple cmake find module for the Nuke NDK
#

set( NUKE_INSTALL_PATH $ENV{NDKDIR} CACHE PATH "Path to folder where Nuke is installed" )

if(EXISTS ${NUKE_INSTALL_PATH})
  get_filename_component( NUKE_VERSION "${NUKE_INSTALL_PATH}" NAME )

  set( NUKE_OSX_SUFFIX ${NUKE_VERSION}.app/Contents/MacOS )
  find_path( NUKE_INCLUDE_DIR
             NAMES DDImage/Op.h
             PATHS ${NUKE_INSTALL_PATH}
             PATH_SUFFIXES include ${NUKE_OSX_SUFFIX}/include )

  if(NUKE_INCLUDE_DIR AND EXISTS "${NUKE_INCLUDE_DIR}/DDImage/ddImageVersionNumbers.h")
    file(STRINGS "${NUKE_INCLUDE_DIR}/DDImage/ddImageVersionNumbers.h" 
         nuke_version_str
         REGEX "^#define[\t ]+kDDImageVersion[\t ]+\".*")

    string(REGEX REPLACE "^#define[\t ]+kDDImageVersion[\t ]+\"([^ \\n]*)\".*"
           "\\1" NUKE_VERSION "${nuke_version_str}")
    unset(nuke_version_str)
  else()
    string(REGEX REPLACE "^Nuke(.*)"
           "\\1" NUKE_VERSION "${NUKE_VERSION}")
  endif()
  string(REGEX REPLACE "^(.*)v.*"
         "\\1" NUKE_API_VERSION "${NUKE_VERSION}")

  find_library(NUKE_LIBRARY
               NAMES DDImage
               PATHS ${NUKE_INSTALL_PATH}
               PATH_SUFFIXES ${NUKE_OSX_SUFFIX})
else()
  if(NOT Nuke_FIND_QUIETLY)
    if("${NUKE_INSTALL_PATH}" STREQUAL "")
      message( STATUS "NUKE_INSTALL_PATH has not been set for cmake, please set the NDKDIR environment variable and clear the cmake cache or use the CMake GUI to specify" )
    else()
      message( STATUS "NUKE_INSTALL_PATH '${NUKE_INSTALL_PATH}' does not exist, please specify by setting the NDKDIR environment variable and clearing the cmake cache or use the CMake GUI to specify" )
    endif()
  endif()
endif()

#
# This is kind of a hack, but it allows us to pass in a flag whether
# we have to copy the DDImage library locally so we can avoid running
# afoul of the nuke Ilm libraries in case we are using a different
# version
#
if(Nuke_FIND_COMPONENTS)
  foreach( component ${Nuke_FIND_COMPONENTS} )
    string( TOUPPER ${component} _COMPONENT )
    set(NUKE_${_COMPONENT} 1)
  endforeach()
endif()

if(NUKE_AVOIDNUKEILMBASE)
  file(COPY ${NUKE_LIBRARY} DESTINATION ${CMAKE_BINARY_DIR})
  get_filename_component( libddimage "${NUKE_LIBRARY}" NAME )
  set(NUKE_LIBRARY ${CMAKE_BINARY_DIR}/${libddimage})
endif()
set(NUKE_LIBRARIES ${NUKE_LIBRARY} )
set(NUKE_INCLUDE_DIRS ${NUKE_INCLUDE_DIR} )
get_filename_component(NUKE_LIBRARY_DIR "${NUKE_LIBRARY}" PATH)
set(NUKE_LIBRARY_DIRS ${NUKE_LIBRARY_DIR})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set NUKE_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Nuke
                                  REQUIRED_VARS NUKE_LIBRARY NUKE_INCLUDE_DIR
                                  VERSION_VAR NUKE_VERSION
                                  FAIL_MESSAGE "Unable to find NUKE NDK library" )

mark_as_advanced(NUKE_INCLUDE_DIR NUKE_LIBRARY)
