#
# A simple cmake find module for Imath
#

find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(PC_ILMBASE QUIET Imath)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  # Under Mac OS, if the user has installed using brew, the package config
  # information is not entirely accurate in that it refers to the
  # true install path but brew maintains links to /usr/local for you
  # which they want you to use
  if(PC_ILMBASE_FOUND AND "${PC_ILMBASE_INCLUDEDIR}" MATCHES "^/usr/local/Cellar.*")
    set(_IlmBase_HINT_INCLUDE /usr/local/include)
    set(_IlmBase_HINT_LIB /usr/local/lib)
  endif()
endif()

if(PC_ILMBASE_FOUND)
  set(IlmBase_CFLAGS ${PC_ILMBASE_CFLAGS_OTHER})
  set(IlmBase_LIBRARY_DIRS ${PC_ILMBASE_LIBRARY_DIRS})
  set(IlmBase_LDFLAGS ${PC_ILMBASE_LDFLAGS_OTHER})
  if("${_IlmBase_HINT_INCLUDE}" STREQUAL "")
    set(_IlmBase_HINT_INCLUDE ${CMAKE_PREFIX_PATH}/include )
    set(_IlmBase_HINT_LIB ${CMAKE_PREFIX_PATH}/lib ${CMAKE_PREFIX_PATH}/lib64 )
  endif()
else()
  if(UNIX)
    set(_IlmBase_HINT_INCLUDE ${CMAKE_PREFIX_PATH}/include )
    set(_IlmBase_HINT_LIB ${CMAKE_PREFIX_PATH}/lib ${CMAKE_PREFIX_PATH}/lib64 )
    set(IlmBase_CFLAGS "-pthread")
    if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    else()
      set(OpenEXR_LDFLAGS "-pthread")
    endif()
  endif()
endif()

find_path(IlmBase_INCLUDE_DIR ImathConfig.h HINTS ${_IlmBase_HINT_INCLUDE} PATH_SUFFIXES Imath )
if(IlmBase_INCLUDE_DIR AND EXISTS "${IlmBase_INCLUDE_DIR}/ImathConfig.h")
    #set(IlmBase_VERSION ${PC_ILMBASE_VERSION})
    if("${IlmBase_VERSION}" STREQUAL "")
      file(STRINGS "${IlmBase_INCLUDE_DIR}/ImathConfig.h" ilmbase_version_str
	   REGEX "^#define[\t ]+IMATH_VERSION_STRING[\t ]+\".*")

      string(REGEX REPLACE "^#define[\t ]+IMATH_VERSION_STRING[\t ]+\"([^ \\n]*)\".*"
	     "\\1" IlmBase_VERSION "${ilmbase_version_str}")
      unset(ilmbase_version_str)
    endif()
endif()

if("${IlmBase_VERSION}" VERSION_LESS "2.0.0")
  set(IlmBase_ALL_LIBRARIES Imath-3_1 Iex-3_1 IlmThread-3_2 IlmThread-3_1)
else()
  set(IlmBase_ALL_LIBRARIES Imath-3_1 Iex-3_1 IlmThread-3_2 IlmThread-3_1)
endif()
foreach(ILMBASE_LIB ${IlmBase_ALL_LIBRARIES})
  string(TOUPPER ${ILMBASE_LIB} _upper_ilmbase_lib)
  find_library(IlmBase_${_upper_ilmbase_lib}_LIBRARY
	       NAMES ${ILMBASE_LIB} lib${ILMBASE_LIB}
	       HINTS ${_IlmBase_HINT_LIB}
  )
  if(IlmBase_${_upper_ilmbase_lib}_LIBRARY)
    set(IlmBase_LIBRARY ${IlmBase_LIBRARY} ${IlmBase_${_upper_ilmbase_lib}_LIBRARY})
    message( STATUS "ILMBASE LIB=${IlmBase_LIBRARY}" )
    mark_as_advanced(IlmBase_${_upper_ilmbase_lib}_LIBRARY)
  endif()
endforeach()

unset(_IlmBase_HINT_INCLUDE)
unset(_IlmBase_HINT_LIB)
set(IlmBase_LIBRARIES ${IlmBase_LIBRARY} )
set(IlmBase_INCLUDE_DIRS ${IlmBase_INCLUDE_DIR}
			 ${CMAKE_PREFIX_PATH}/include/OpenEXR )

if(NOT PC_ILMBASE_FOUND)
       get_filename_component(IlmBase_LDFLAGS_OTHER ${IlmBase_IMATH-3_1_LIBRARY} PATH)
if(WIN32)
	set(IlmBase_LDFLAGS_OTHER -LIBPATH:${IlmBase_LDFLAGS_OTHER})
else(WIN32)
	set(IlmBase_LDFLAGS_OTHER -L${IlmBase_LDFLAGS_OTHER})
endif(WIN32)
endif()

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set IlmBase_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(IlmBase
				  REQUIRED_VARS IlmBase_LIBRARY IlmBase_INCLUDE_DIR
				  VERSION_VAR IlmBase_VERSION
				  FAIL_MESSAGE "Unable to find IlmBase libraries" )


# older versions of cmake don't support FOUND_VAR to find_package_handle
# so just do it the hard way...
if(ILMBASE_FOUND AND NOT IlmBase_FOUND)
  set(IlmBase_FOUND 1)
endif()

mark_as_advanced(IlmBase_INCLUDE_DIR IlmBase_LIBRARY )
