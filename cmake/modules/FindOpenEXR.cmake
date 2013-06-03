#
# A simple cmake find module for OpenEXR
#

find_package(PkgConfig)
pkg_check_modules(PC_OPENEXR QUIET OpenEXR)

if(PC_OPENEXR_FOUND)
  set(OpenEXR_CFLAGS ${PC_OPENEXR_CFLAGS_OTHER})
  set(OpenEXR_LIBRARY_DIRS ${PC_OPENEXR_LIBRARY_DIRS})
  set(OpenEXR_LDFLAGS ${PC_OPENEXR_LDFLAGS_OTHER})
else()
  if(UNIX)
    set(OpenEXR_CFLAGS "-pthread")
    set(OpenEXR_LDFLAGS "-pthread")
  endif()
endif()

find_path(OpenEXR_INCLUDE_DIR OpenEXRConfig.h
          HINTS ${PC_OPENEXR_INCLUDEDIR} ${PC_OPENEXR_INCLUDE_DIRS}
          PATH_SUFFIXES OpenEXR )
if(OpenEXR_INCLUDE_DIR AND EXISTS "${OpenEXR_INCLUDE_DIR}/OpenEXRConfig.h")
    file(STRINGS "${OpenEXR_INCLUDE_DIR}/OpenEXRConfig.h" openexr_version_str
         REGEX "^#define[\t ]+OPENEXR_VERSION_STRING[\t ]+\".*")

    string(REGEX REPLACE "^#define[\t ]+OPENEXR_VERSION_STRING[\t ]+\"([^ \\n]*)\".*"
           "\\1" OpenEXR_VERSION "${openexr_version_str}")
    unset(openexr_version_str)
endif()

find_library(OpenEXR_LIBRARY
             NAMES IlmImf libIlmImf
             HINTS ${PC_OPENEXR_LIBDIR} ${PC_OPENEXR_LIBRARY_DIRS})

find_package(IlmBase QUIET)

set(OpenEXR_LIBRARIES ${OpenEXR_LIBRARY} ${IlmBase_LIBRARIES} )
set(OpenEXR_INCLUDE_DIRS ${OpenEXR_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set OpenEXR_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(OpenEXR
                                  FOUND_VAR OpenEXR_FOUND
                                  REQUIRED_VARS OpenEXR_LIBRARY OpenEXR_INCLUDE_DIR
                                  VERSION_VAR OpenEXR_VERSION
                                  FAIL_MESSAGE "Unable to find OpenEXR library" )

mark_as_advanced(OpenEXR_INCLUDE_DIR OpenEXR_LIBRARY )
