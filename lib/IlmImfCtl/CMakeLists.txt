include_directories( "${CMAKE_CURRENT_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}/lib/IlmCtl" "${PROJECT_SOURCE_DIR}/lib/IlmCtlMath" "${PROJECT_SOURCE_DIR}/lib/IlmCtlSimd" )
include_directories( ${OpenEXR_INCLUDE_DIRS} )
link_directories( ${OpenEXR_LIBRARY_DIRS} )

add_library( IlmImfCtl ${DO_SHARED}
  ImfCtlApplyTransforms.cpp
  ImfCtlCopyFunctionArg.cpp
)

target_link_libraries( IlmImfCtl IlmCtl ${OpenEXR_LIBRARIES} ${IlmBase_LIBRARIES} )

install( FILES ImfCtlApplyTransforms.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/OpenEXR )

export( TARGETS IlmImfCtl IlmCtl FILE "${PROJECT_BINARY_DIR}/CTLLibraryDepends.cmake" )

install( TARGETS IlmImfCtl DESTINATION ${CMAKE_INSTALL_LIBDIR} )
