# Until we get some of these modules into the upstream packages, put them here
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_INSTALL_PREFIX}/share/CMake")

find_package( PkgConfig QUIET )

find_package( IlmBase QUIET )
if(IlmBase_FOUND)
  message( STATUS "found IlmBase, version ${IlmBase_VERSION}" )
  ###
  ### Everyone (well, except for DPX) uses IlmBase, so
  ### make that a global setting
  ###
  include_directories( ${IlmBase_INCLUDE_DIRS} )
  link_directories( ${IlmBase_LIBRARY_DIRS} )
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${IlmBase_CFLAGS}" )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${IlmBase_CFLAGS}" )
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${IlmBase_LDFLAGS}" )
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${IlmBase_LDFLAGS}" )
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${IlmBase_LDFLAGS}" )
else()
  message( SEND_ERROR "IlmBase not found, please set the include and library paths manually" )
endif()

find_package( TIFF QUIET )
if (TIFF_FOUND)
  message( STATUS "found TIFF, version ${TIFF_VERSION_STRING}" )
  # Make the variables the same as if pkg-config finds them
  set(TIFF_INCLUDE_DIRS ${TIFF_INCLUDE_DIR})
  set(TIFF_LIBRARY_DIR ${TIFF_LIBRARY})
else()
  if ( PKG_CONFIG_FOUND )
    pkg_search_module( TIFF libtiff libtiff-4 )
    if (TIFF_FOUND)
      message( STATUS "found TIFF via pkg-config, version ${TIFF_VERSION}" )
    endif()
  else()
    message( WARNING "Unable to find TIFF libraries, disabling" )
  endif()
endif()

find_package( OpenEXR QUIET )
if (OpenEXR_FOUND)
  message( STATUS "Found OpenEXR, version ${OpenEXR_VERSION}" )
else()
  message( WARNING "Unable to find OpenEXR libraries, disabling" )
endif()

find_package( AcesContainer )
if (AcesContainer_FOUND)
  message( STATUS "Found AcesContainer, version ${AcesContainer_VERSION}" )
else()
  if ( PKG_CONFIG_FOUND )
    pkg_check_modules( AcesContainer AcesContainer )
  else()
    message( WARNING "Unable to find AcesContainer libraries, disabling" )
  endif()
endif()
