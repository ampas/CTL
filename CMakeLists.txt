cmake_minimum_required(VERSION 3.12)
project(CTL VERSION 1.5.3)
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
	${CMAKE_MODULE_PATH}
)

add_definitions( -DPACKAGE="${PROJECT_NAME}" -DVERSION="${PROJECT_VERSION}" )

option(CTL_BUILD_TESTS "Build the unit tests" ON)
option(CTL_BUILD_TOOLS "Build the utility commands (ctlrender, etc)" ON)
option(BUILD_SHARED_LIBS "Build all libraries as shared" OFF )

set(CMAKE_CXX_FLAGS_ASAN
  "-fsanitize=address -fno-optimize-sibling-calls -fsanitize-address-use-after-scope -fno-omit-frame-pointer -g -O1"
  CACHE STRING "Flags used by the C++ compiler during AddressSanitizer builds."
  FORCE)

if( NOT DEFINED CMAKE_BUILD_TYPE )
  MESSAGE( STATUS "To use Release mode, use \"cmake .. -DCMAKE_BUILD_TYPE=Release\"" )
  MESSAGE( STATUS "To use AddressSanitizer, use \"cmake .. -DCMAKE_BUILD_TYPE=asan\"" )
elseif( CMAKE_BUILD_TYPE STREQUAL "Release" )
  MESSAGE( STATUS "Using Release mode - CMAKE_BUILD_TYPE : \"${CMAKE_BUILD_TYPE}\"")
elseif( CMAKE_BUILD_TYPE STREQUAL "asan" )
  MESSAGE( STATUS "Using AddressSanitizer - CMAKE_BUILD_TYPE : \"${CMAKE_BUILD_TYPE}\"")
else()
  MESSAGE( STATUS "CMAKE_BUILD_TYPE : \"${CMAKE_BUILD_TYPE}\"")
  MESSAGE( STATUS "To use Release mode, use \"cmake .. -DCMAKE_BUILD_TYPE=Release\"" )
  MESSAGE( STATUS "To use AddressSanitizer, use \"cmake .. -DCMAKE_BUILD_TYPE=asan\"" )
endif()

# Set a default build type if none was specified
set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# ZLIB is automatically added by OpenEXR 3.x but needs to be added for OpenEXR 2.x
find_package(ZLIB)
if(TARGET ZLIB::ZLIB)
	message( STATUS "found ZLIB" )
	message( STATUS "	ZLIB_VERSION : ${ZLIB_VERSION}" )
	message( STATUS "	ZLIB_INCLUDE_DIRS : ${ZLIB_INCLUDE_DIRS}" )
	message( STATUS "	ZLIB_LIBRARIES : ${ZLIB_LIBRARIES}" )
	message( STATUS "	ZLIB_INCLUDE_DIR : ${ZLIB_INCLUDE_DIR}" )
	message( STATUS "	ZLIB_LIBRARY : ${ZLIB_LIBRARY}" )
endif()

find_package(OpenEXR 3 CONFIG QUIET)
if(OpenEXR_FOUND)
  message(STATUS "Found OpenEXR ${OpenEXR_VERSION}")
else()
  find_package(IlmBase REQUIRED)
  find_package(OpenEXR 2 REQUIRED)
endif()

add_subdirectory(lib)
add_subdirectory(doc)
if (CTL_BUILD_TOOLS)
  add_subdirectory(ctlrender)
  add_subdirectory(OpenEXR_CTL)
endif()

if (CTL_BUILD_TESTS)
  enable_testing()
  add_subdirectory(unittest)
endif()

