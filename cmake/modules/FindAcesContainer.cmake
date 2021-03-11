# Module to find AcesContainer or to build it if unavailable

# Try to use the default CMake ZLib finder
find_package(AcesContainer QUIET CONFIG)
if (AcesContainer_FOUND)
	# Try to make a modern CMake target to use
	add_library(AcesContainer::AcesContainer UNKNOWN IMPORTED GLOBAL)
	find_file(AcesContainer_LIBRARY
		NAMES
			${AcesContainer_LIBRARIES}.${CMAKE_STATIC_LIBRARY_SUFFIX}
			${AcesContainer_LIBRARIES}.${CMAKE_SHARED_LIBRARY_SUFFIX}
		PATH
			${AcesContainer_LIBRARY_DIRS}
	)
	if (NOT AcesContainer_LIBRARY)
		message(FATAL_ERROR "Error configuring AcesContainer. Please notify CTL maintainers.")
	endif()
	set_target_properties(AcesContainer::AcesContainer
		PROPERTIES
			IMPORTED_LOCATION ${AcesContainer_LIBRARY}
			INTERFACE_INCLUDE_DIRECTORIES ${AcesContainer_INCLUDE_DIRS}
	)
	return()
endif()

# If we got to here, then we need to build AcesContainer

include(GNUInstallDirs)
include(ExternalProject)
add_library(AcesContainer::AcesContainer STATIC IMPORTED GLOBAL)
set(ACESCONTAINER_MAJOR_VERSION "1")
set(ACESCONTAINER_MINOR_VERSION "0")
set(ACESCONTAINER_PATCH_VERSION "2")
set(ACESCONTAINER_VERSION "${ACESCONTAINER_MAJOR_VERSION}.${ACESCONTAINER_MINOR_VERSION}.${ACESCONTAINER_PATCH_VERSION}")

set(EXT_DIST_ROOT "${CMAKE_BINARY_DIR}/ext/dist")
set(EXT_BUILD_ROOT "${CMAKE_BINARY_DIR}/ext/build")

set(ACESCONTAINER_INCLUDE_DIR "${EXT_DIST_ROOT}/include/aces")
set(ACESCONTAINER_LIB_PREFIX "${EXT_DIST_ROOT}/${CMAKE_INSTALL_LIBDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}")
set(ACESCONTAINER_LIB_SUFFIX "${CMAKE_STATIC_LIBRARY_SUFFIX}")

set(ACESCONTAINER_LIBRARY "${ACESCONTAINER_LIB_PREFIX}AcesContainer${ACESCONTAINER_LIB_SUFFIX}")

set(ACESCONTAINER_CMAKE_ARGS
	-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
	#-DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
	-DCMAKE_INSTALL_PREFIX=${EXT_DIST_ROOT}
	-DBUILD_SHARED_LIBS=OFF
)
file(MAKE_DIRECTORY ${ACESCONTAINER_INCLUDE_DIR})

ExternalProject_Add(ACESCONTAINER_BUILD
	GIT_REPOSITORY "https://github.com/ampas/aces_container.git"
	GIT_TAG "v${ACESCONTAINER_VERSION}"
	GIT_SHALLOW TRUE
	PREFIX "${EXT_BUILD_ROOT}/acescontainer"
	CMAKE_ARGS ${ACESCONTAINER_CMAKE_ARGS}
	UPDATE_COMMAND "" # Skip re-checking the tag every build
	BUILD_COMMAND
		${CMAKE_COMMAND} --build .
						 --config $<CONFIG>
						 --target AcesContainer
	INSTALL_COMMAND
		${CMAKE_COMMAND} -DBUILD_TYPE=$<CONFIG> -P "cmake_install.cmake"
	EXCLUDE_FROM_ALL TRUE
)

add_dependencies(AcesContainer::AcesContainer ACESCONTAINER_BUILD)
set_target_properties(AcesContainer::AcesContainer
	PROPERTIES
		IMPORTED_LOCATION ${ACESCONTAINER_LIBRARY}
		INTERFACE_INCLUDE_DIRECTORIES ${ACESCONTAINER_INCLUDE_DIR}
)
