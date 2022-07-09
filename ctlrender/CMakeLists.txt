
if (TIFF_FOUND)
add_definitions( -DHAVE_LIBTIFF=1 )
include_directories( ${TIFF_INCLUDE_DIRS} )
link_directories( ${TIFF_LIBRARY_DIRS} )
endif()

if ( OpenEXR_FOUND )
add_definitions( -DHAVE_OPENEXR=1 )
include_directories( ${OpenEXR_INCLUDE_DIRS} )
link_directories( ${OpenEXR_LIBRARY_DIRS} )
endif()

if ( AcesContainer_FOUND )
add_definitions( -DHAVE_ACESFILE=1 )
include_directories( ${AcesContainer_INCLUDE_DIRS} )
link_directories( ${AcesContainer_LIBRARY_DIRS} )
endif()

include_directories( "${CMAKE_CURRENT_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}/lib/IlmCtl" "${PROJECT_SOURCE_DIR}/lib/IlmCtlMath" "${PROJECT_SOURCE_DIR}/lib/IlmCtlSimd" "${PROJECT_SOURCE_DIR}/lib/dpx" )

add_executable( ctlrender
  main.cc
  transform.cc
  usage.cc
  aces_file.cc
  dpx_file.cc
  exr_file.cc
  tiff_file.cc
  format.cc
  compression.cc
)

target_link_libraries( ctlrender IlmCtlSimd IlmCtlMath IlmCtl ctldpx ${IlmBase_LIBRARIES} )
target_link_libraries( ctlrender ${IlmBase_LDFLAGS_OTHER} )
if (TIFF_FOUND)
target_link_libraries( ctlrender ${TIFF_LIBRARIES} )
target_link_libraries( ctlrender ${TIFF_LDFLAGS_OTHER} )
endif()
if (OpenEXR_FOUND)
target_link_libraries( ctlrender ${OpenEXR_LIBRARIES} )
target_link_libraries( ctlrender ${OpenEXR_LDFLAGS_OTHER} )
endif()
if (AcesContainer_FOUND)
target_link_libraries( ctlrender ${AcesContainer_LIBRARIES} )
target_link_libraries( ctlrender ${AcesContainer_LDFLAGS_OTHER} )
endif()

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set_target_properties(ctlrender PROPERTIES INSTALL_RPATH "@loader_path/../lib")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

install( TARGETS ctlrender DESTINATION bin )
