include_directories( "${CMAKE_CURRENT_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}/lib/IlmCtl" "${PROJECT_SOURCE_DIR}/lib/IlmCtlMath" "${PROJECT_SOURCE_DIR}/lib/IlmCtlSimd" )

add_library( IlmImfCtl ${DO_SHARED}
  ImfCtlApplyTransforms.cpp
  ImfCtlCopyFunctionArg.cpp
)

target_link_libraries( IlmImfCtl IlmCtl IlmImf Iex IlmThread Half )

install( FILES ImfCtlApplyTransforms.h DESTINATION include/OpenEXR )

export( TARGETS IlmImfCtl IlmCtl FILE "${PROJECT_BINARY_DIR}/CTLLibraryDepends.cmake" )

install( TARGETS IlmImfCtl DESTINATION lib )
