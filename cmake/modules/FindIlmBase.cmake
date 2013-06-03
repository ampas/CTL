#
# A simple cmake find module for IlmBase
#

find_package(PkgConfig)
pkg_check_modules(PC_ILMBASE QUIET IlmBase)

if(PC_ILMBASE_FOUND)
  set(IlmBase_CFLAGS ${PC_ILMBASE_CFLAGS_OTHER})
  set(IlmBase_LIBRARY_DIRS ${PC_ILMBASE_LIBRARY_DIRS})
  set(IlmBase_LDFLAGS ${PC_ILMBASE_LDFLAGS_OTHER})
else()
  if(UNIX)
    set(IlmBase_CFLAGS "-pthread")
    set(IlmBase_LDFLAGS "-pthread")
  endif()
endif()

find_path(IlmBase_INCLUDE_DIR IlmBaseConfig.h
          HINTS ${PC_ILMBASE_INCLUDEDIR} ${PC_ILMBASE_INCLUDE_DIRS}
          PATH_SUFFIXES OpenEXR )
if(IlmBase_INCLUDE_DIR AND EXISTS "${IlmBase_INCLUDE_DIR}/IlmBaseConfig.h")
    file(STRINGS "${IlmBase_INCLUDE_DIR}/IlmBaseConfig.h" ilmbase_version_str
         REGEX "^#define[\t ]+ILMBASE_VERSION_STRING[\t ]+\".*")

    string(REGEX REPLACE "^#define[\t ]+ILMBASE_VERSION_STRING[\t ]+\"([^ \\n]*)\".*"
           "\\1" IlmBase_VERSION "${ilmbase_version_str}")
    unset(ilmbase_version_str)
endif()

set(IlmBase_ALL_LIBRARIES Imath Half Iex IexMath IlmThread)
foreach(ILMBASE_LIB ${IlmBase_ALL_LIBRARIES})
  string(TOUPPER ${ILMBASE_LIB} _upper_ilmbase_lib)
  find_library(IlmBase_${_upper_ilmbase_lib}_LIBRARY
               NAMES ${ILMBASE_LIB} lib${ILMBASE_LIB}
               HINTS ${PC_ILMBASE_LIBDIR} ${PC_ILMBASE_LIBRARY_DIRS})
  if(IlmBase_${_upper_ilmbase_lib}_LIBRARY)
    set(IlmBase_LIBRARY ${IlmBase_LIBRARY} ${IlmBase_${_upper_ilmbase_lib}_LIBRARY})
    mark_as_advanced(IlmBase_${_upper_ilmbase_lib}_LIBRARY)
  endif()
endforeach()

set(IlmBase_LIBRARIES ${IlmBase_LIBRARY} )
set(IlmBase_INCLUDE_DIRS ${IlmBase_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set IlmBase_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(IlmBase
                                  FOUND_VAR IlmBase_FOUND
                                  REQUIRED_VARS IlmBase_LIBRARY IlmBase_INCLUDE_DIR
                                  VERSION_VAR IlmBase_VERSION
                                  FAIL_MESSAGE "Unable to find IlmBase libraries" )

mark_as_advanced(IlmBase_INCLUDE_DIR IlmBase_LIBRARY )
