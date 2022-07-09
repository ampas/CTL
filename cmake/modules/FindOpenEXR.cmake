#
# A simple cmake find module for OpenEXR
#

find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(PC_OPENEXR QUIET OpenEXR)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  # Under Mac OS, if the user has installed using brew, the package config
  # information is not entirely accurate in that it refers to the
  # true install path but brew maintains links to /usr/local for you
  # which they want you to use
  if(PC_OPENEXR_FOUND AND "${PC_OPENEXR_INCLUDEDIR}" MATCHES ".*/Cellar/.*")
    set(_OpenEXR_HINT_INCLUDE /usr/local/include)
    set(_OpenEXR_HINT_LIB /usr/local/lib)
  endif()
endif()

if(PC_OPENEXR_FOUND)
  set(OpenEXR_CFLAGS ${PC_OPENEXR_CFLAGS_OTHER})
  set(OpenEXR_LIBRARY_DIRS ${PC_OPENEXR_LIBRARY_DIRS})
  set(OpenEXR_LDFLAGS ${PC_OPENEXR_LDFLAGS_OTHER})
  if("${_OpenEXR_HINT_INCLUDE}" STREQUAL "")
    set(_OpenEXR_HINT_INCLUDE ${PC_OPENEXR_INCLUDEDIR} ${PC_OPENEXR_INCLUDE_DIRS})
    set(_OpenEXR_HINT_LIB ${PC_OPENEXR_LIBDIR} ${PC_OPENEXR_LIBRARY_DIRS})
  endif()
else()
  if(UNIX)
    if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    else()
      set(OpenEXR_CFLAGS "-pthread")
      set(OpenEXR_LDFLAGS "-pthread")
    endif()
  endif()
endif()

find_path(OpenEXR_INCLUDE_DIR OpenEXRConfig.h
          HINTS ${_OpenEXR_HINT_INCLUDE}
          PATH_SUFFIXES OpenEXR )
if(OpenEXR_INCLUDE_DIR AND EXISTS "${OpenEXR_INCLUDE_DIR}/OpenEXRConfig.h")
    set(OpenEXR_VERSION ${PC_OPENEXR_VERSION})

    if("${OpenEXR_VERSION}" STREQUAL "")
      file(STRINGS "${OpenEXR_INCLUDE_DIR}/OpenEXRConfig.h" openexr_version_str
           REGEX "^#define[\t ]+OPENEXR_VERSION_STRING[\t ]+\".*")

      string(REGEX REPLACE "^#define[\t ]+OPENEXR_VERSION_STRING[\t ]+\"([^ \\n]*)\".*"
             "\\1" OpenEXR_VERSION "${openexr_version_str}")
      unset(openexr_version_str)
    endif()
endif()

find_library(OpenEXR_LIBRARY NAMES IlmImf libIlmImf HINTS ${_OpenEXR_HINT_LIB})

find_package(IlmBase QUIET)

unset(_OpenEXR_HINT_INCLUDE)
unset(_OpenEXR_HINT_LIB)

set(OpenEXR_LIBRARIES ${OpenEXR_LIBRARY} ${IlmBase_LIBRARIES} )
set(OpenEXR_INCLUDE_DIRS ${OpenEXR_INCLUDE_DIR} )

if(NOT PC_OPENEXR_FOUND)
get_filename_component(OpenEXR_LDFLAGS_OTHER ${OpenEXR_LIBRARY} PATH)
set(OpenEXR_LDFLAGS_OTHER -L${OpenEXR_LDFLAGS_OTHER})
endif()

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set OpenEXR_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(OpenEXR
                                  REQUIRED_VARS OpenEXR_LIBRARY OpenEXR_INCLUDE_DIR
                                  VERSION_VAR OpenEXR_VERSION
                                  FAIL_MESSAGE "Unable to find OpenEXR library" )

# older versions of cmake don't support FOUND_VAR to find_package_handle
# so just do it the hard way...
if(OPENEXR_FOUND AND NOT OpenEXR_FOUND)
  set(OpenEXR_FOUND 1)
endif()

mark_as_advanced(OpenEXR_INCLUDE_DIR OpenEXR_LIBRARY )
