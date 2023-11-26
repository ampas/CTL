# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#[=======================================================================[.rst:
FindAcesContainer
-------

Finds the AcesContainer library.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``AcesContainer::AcesContainer``
  The AcesContainer library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``AcesContainer_FOUND``
  True if the system has the AcesContainer library.
``AcesContainer_VERSION``
  The version of the AcesContainer library which was found.
``AcesContainer_INCLUDE_DIRS``
  Include directories needed to use AcesContainer.
``AcesContainer_LIBRARIES``
  Libraries needed to link to AcesContainer.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``AcesContainer_INCLUDE_DIR``
  The directory containing ``AcesContainer.h``.
``AcesContainer_LIBRARY``
  The path to the AcesContainer library.

#]=======================================================================]

cmake_minimum_required(VERSION 3.12)
include(GNUInstallDirs)

message(STATUS "running FindAcesContainer.cmake")

# https://cmake.org/cmake/help/latest/manual/cmake-developer.7.html#find-modules

find_package(PkgConfig)
pkg_check_modules(PC_AcesContainer QUIET AcesContainer)

find_path(AcesContainer_INCLUDE_DIR
  NAMES aces_Writer.h
  #PATHS ${PC_AcesContainer_INCLUDE_DIRS}
  PATHS /usr/local/include/
  PATH_SUFFIXES aces
)
find_library(AcesContainer_LIBRARY
  NAMES AcesContainer
  #PATHS ${PC_AcesContainer_LIBRARY_DIRS}
  PATHS /usr/local/lib/
)

find_library(AcesContainer_LIBRARY_RELEASE
  NAMES AcesContainer
  PATHS ${PC_AcesContainer_LIBRARY_DIRS}/Release
)
find_library(AcesContainer_LIBRARY_DEBUG
  NAMES AcesContainer
  PATHS ${PC_AcesContainer_LIBRARY_DIRS}/Debug
)

include(SelectLibraryConfigurations)
select_library_configurations(AcesContainer)

set(AcesContainer_VERSION ${PC_AcesContainer_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(AcesContainer
  FOUND_VAR AcesContainer_FOUND
  REQUIRED_VARS
    AcesContainer_LIBRARY
    AcesContainer_INCLUDE_DIR
  VERSION_VAR AcesContainer_VERSION
)

if(AcesContainer_FOUND)
  set(AcesContainer_LIBRARIES ${AcesContainer_LIBRARY})
  set(AcesContainer_INCLUDE_DIRS ${AcesContainer_INCLUDE_DIR})
  set(AcesContainer_DEFINITIONS ${PC_AcesContainer_CFLAGS_OTHER})
endif()

if(AcesContainer_FOUND AND NOT TARGET AcesContainer::AcesContainer)
  add_library(AcesContainer::AcesContainer UNKNOWN IMPORTED)
  set_target_properties(AcesContainer::AcesContainer PROPERTIES
    IMPORTED_LOCATION "${AcesContainer_LIBRARY}"
    INTERFACE_COMPILE_OPTIONS "${PC_AcesContainer_CFLAGS_OTHER}"
    INTERFACE_INCLUDE_DIRECTORIES "${AcesContainer_INCLUDE_DIR}"
  )
endif()

if(AcesContainer_FOUND)
  if (NOT TARGET AcesContainer::AcesContainer)
    add_library(AcesContainer::AcesContainer UNKNOWN IMPORTED)
  endif()
  if (AcesContainer_LIBRARY_RELEASE)
    set_property(TARGET AcesContainer::AcesContainer APPEND PROPERTY
      IMPORTED_CONFIGURATIONS RELEASE
    )
    set_target_properties(AcesContainer::AcesContainer PROPERTIES
      IMPORTED_LOCATION_RELEASE "${AcesContainer_LIBRARY_RELEASE}"
    )
  endif()
  if (AcesContainer_LIBRARY_DEBUG)
    set_property(TARGET AcesContainer::AcesContainer APPEND PROPERTY
      IMPORTED_CONFIGURATIONS DEBUG
    )
    set_target_properties(AcesContainer::AcesContainer PROPERTIES
      IMPORTED_LOCATION_DEBUG "${AcesContainer_LIBRARY_DEBUG}"
    )
  endif()
  set_target_properties(AcesContainer::AcesContainer PROPERTIES
    INTERFACE_COMPILE_OPTIONS "${PC_AcesContainer_CFLAGS_OTHER}"
    INTERFACE_INCLUDE_DIRECTORIES "${AcesContainer_INCLUDE_DIR}"
  )
endif()

mark_as_advanced(
  AcesContainer_INCLUDE_DIR
  AcesContainer_LIBRARY
)

# compatibility variables
set(AcesContainer_VERSION_STRING ${AcesContainer_VERSION})



