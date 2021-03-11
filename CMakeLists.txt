cmake_minimum_required(VERSION 3.1)
project(CTL VERSION 1.5.1)
enable_testing()
include(GNUInstallDirs)

set(CMAKE_MODULE_PATH
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
	${CMAKE_MODULE_PATH}
)

option(CTL_BUILD_TESTS "Build the unit tests" OFF)
option(CTL_BUILD_TOOLS "Build the utility commands (ctlrender, etc)" ON)

find_package(Imath REQUIRED)
find_package(OpenEXR REQUIRED)

add_subdirectory(lib)
add_subdirectory(doc)
if (CTL_BUILD_TOOLS)
	add_subdirectory(ctlrender)
endif()

if (CTL_BUILD_TESTS)
	add_subdirectory(unittest)
endif()

