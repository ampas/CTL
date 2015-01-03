include_directories( "${CMAKE_CURRENT_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}/lib/IlmCtl" "${PROJECT_SOURCE_DIR}/lib/IlmCtlMath" "${PROJECT_SOURCE_DIR}/lib/IlmCtlSimd" )
include_directories( ${OpenEXR_INCLUDE_DIRS} )
link_directories( ${OpenEXR_LIBRARY_DIRS} )

add_library( IlmImfCtl ${DO_SHARED}
  ImfCtlApplyTransforms.cpp
  ImfCtlCopyFunctionArg.cpp
)

target_link_libraries( IlmImfCtl IlmCtl IlmImf Iex IlmThread Half )

install( FILES ImfCtlApplyTransforms.h DESTINATION include/OpenEXR )

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set_target_properties(IlmImfCtl PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

export( TARGETS IlmImfCtl IlmCtl FILE "${PROJECT_BINARY_DIR}/CTLLibraryDepends.cmake" )

install( TARGETS IlmImfCtl DESTINATION lib )
