
set( NUKE_INSTALL_PATH $ENV{NDKDIR} CACHE PATH "Path to install directory of Nuke" )

# TODO: replace with FindNuke.cmake
get_filename_component( NUKE_VERSION "${NUKE_INSTALL_PATH}" NAME )
set( NUKE_OSX_SUFFIX ${NUKE_VERSION}.app/Contents/MacOS )
find_path(  NUKE_INCLUDE_DIR
            NAMES DDImage/Op.h
            PATHS ${NUKE_INSTALL_PATH}
            PATH_SUFFIXES include ${NUKE_OSX_SUFFIX}/include )
find_library( NUKE_LIBRARIES DDImage
              PATHS ${NUKE_INSTALL_PATH} 
              PATH_SUFFIXES ${NUKE_OSX_SUFFIX} )
GET_FILENAME_COMPONENT( NUKE_LIBRARY_DIR ${NUKE_LIBRARIES} PATH )

if( "${NUKE_INSTALL_PATH}" STREQUAL "" )
    message(WARNING "NUKE_INSTALL_PATH not defined, skipping Nuke_CTL")
    return()
else()
    if( NOT EXISTS "${NUKE_INCLUDE_DIR}/DDImage/op.h" )
        message(WARNING "Nuke install not verified. Skipping Nuke_CTL")
        return()
    endif()
    set( NUKE_API_VERSION "" )
    try_run(NUKE_VERSION_EXITCODE NUKE_VERSION_COMPILED
        ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/NukeVersion.cpp
        COMPILE_DEFINITIONS "-I${NUKE_INCLUDE_DIR}"
        RUN_OUTPUT_VARIABLE NUKE_API_VERSION)
    message(STATUS "NUKE_API_VERSION: ${NUKE_API_VERSION}")
endif()

set( DEFAULT_PLUGIN_DIR "" )
if( "$ENV{NUKE_PATH}" STREQUAL "" )
    set( DEFAULT_PLUGIN_DIR $ENV{HOME}/.nuke )
else()
    set( DEFAULT_PLUGIN_DIR $ENV{NUKE_PATH} )
endif()
set( DEFAULT_PLUGIN_DIR ${DEFAULT_PLUGIN_DIR}/plugins/${NUKE_API_VERSION} )
set( INSTALL_NUKE_PLUGIN_DIR ${DEFAULT_PLUGIN_DIR} CACHE PATH "Install directory for Nuke plugins" )

if( NOT DEFINED INSTALL_NUKE_PLUGIN_DIR )
message(WARNING 
 " No install path defined for nuke plugins
   please define NUKE_PATH or INSTALL_NUKE_PLUGIN_DIR
   skipping Nuke_CTL")
return()
else()
message(STATUS "Nuke plugins will be installed in ${INSTALL_NUKE_PLUGIN_DIR}")
message("Don't forget to add this path to your init.py file")
endif()

include_directories( "${CMAKE_CURRENT_SOURCE_DIR}" 
                     "${NUKE_INCLUDE_DIR}"
                     "${PROJECT_SOURCE_DIR}/lib/IlmCtl" 
                     "${PROJECT_SOURCE_DIR}/lib/IlmCtlMath" 
                     "${PROJECT_SOURCE_DIR}/lib/IlmCtlSimd" 
                     "${PROJECT_SOURCE_DIR}/lib/IlmImfCtl" )

add_library( NukeCtl MODULE
    NukeCtl.cpp
	NukeInputParameters.cpp
	NukeTransform.cpp
)

link_directories( ${NUKE_LIBRARY_DIR} )
target_link_libraries( NukeCtl ${NUKE_LIBRARIES} )

target_link_libraries( NukeCtl IlmCtl IlmCtlMath IlmCtlSimd IlmThread Half Iex )
target_link_libraries( NukeCtl ${IlmBase_LIBRARIES} ${IlmBase_LDFLAGS_OTHER} )

install( TARGETS NukeCtl DESTINATION ${INSTALL_NUKE_PLUGIN_DIR} )

