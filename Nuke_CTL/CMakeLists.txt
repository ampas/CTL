
# NB: We use a fake component to indicate that we want to avoid
# the IlmBase / OpenEXR libraries in the Nuke folder itself
find_package(Nuke QUIET COMPONENTS AvoidNukeIlmBase)
if(NUKE_FOUND)
  message( STATUS "NUKE API Version: ${NUKE_API_VERSION}" )
  set( DEFAULT_PLUGIN_DIR "" )
  if( "$ENV{NUKE_PATH}" STREQUAL "" )
      set( DEFAULT_PLUGIN_DIR $ENV{HOME}/.nuke )
  else()
      set( DEFAULT_PLUGIN_DIR $ENV{NUKE_PATH} )
  endif()

  # The line below doesn't make sense for Nuke 8.0 or greater. The version-specific plugin dir is now
  #   /Library/Application Support/Nuke/8.0/plugins/ [for Mac OS X]
  #   /usr/local/Nuke/8.0/plugins/ [for linux]
  #   C:\Program Files\Common Files\Nuke\8.0\plugins or C:\Program Files (x86)\Common Files\Nuke\8.0\plugins [for Windows]
  # set( DEFAULT_PLUGIN_DIR ${DEFAULT_PLUGIN_DIR}/plugins/${NUKE_API_VERSION} )

  set( INSTALL_NUKE_PLUGIN_DIR ${DEFAULT_PLUGIN_DIR} CACHE PATH "Location to install Nuke plugins" )

  if( NOT DEFINED INSTALL_NUKE_PLUGIN_DIR )
    message(WARNING " No install path defined for nuke plugins
   please define NUKE_PATH or INSTALL_NUKE_PLUGIN_DIR
   skipping Nuke_CTL")
    return()
  else()
    message(STATUS "Nuke plugins will be installed in ${INSTALL_NUKE_PLUGIN_DIR}")
    message("Don't forget to add this path to your init.py file")
  endif()

  include_directories("${CMAKE_CURRENT_SOURCE_DIR}" 
                      "${NUKE_INCLUDE_DIRS}"
                      "${PROJECT_SOURCE_DIR}/lib/IlmCtl" 
                      "${PROJECT_SOURCE_DIR}/lib/IlmCtlMath" 
                      "${PROJECT_SOURCE_DIR}/lib/IlmCtlSimd" 
                      "${PROJECT_SOURCE_DIR}/lib/IlmImfCtl" )

  add_library(NukeCtl SHARED
    NukeCtl.cpp
    NukeCtlUtils.cpp
    NukeCtlChanArgMap.cpp
    NukeCtlTransform.cpp
  )

  set_target_properties(NukeCtl PROPERTIES PREFIX "")
  set_target_properties(NukeCtl PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

  target_link_libraries( NukeCtl IlmCtl IlmCtlMath IlmCtlSimd IlmThread Half Iex )
  target_link_libraries( NukeCtl ${IlmBase_LIBRARIES} ${IlmBase_LDFLAGS_OTHER} )
  link_directories( ${NUKE_LIBRARY_DIR} )
  target_link_libraries( NukeCtl ${NUKE_LIBRARIES} )

  install( TARGETS NukeCtl DESTINATION ${INSTALL_NUKE_PLUGIN_DIR} )
else()
  message( WARNING "NUKE not found, check NUKE_INSTALL_PATH setting or NDKDIR env. var, NukeCTL plugin disabled" )
endif()
