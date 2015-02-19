include_directories( "${CMAKE_CURRENT_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}/lib/IlmCtl" "${PROJECT_SOURCE_DIR}/lib/IlmCtlMath" "${PROJECT_SOURCE_DIR}/lib/IlmCtlSimd" )

if(WIN32)
  add_definitions( -DOPENEXR_DLL )
endif(WIN32)

include_directories( ${OpenEXR_INCLUDE_DIRS} )
link_directories( ${OpenEXR_LIBRARY_DIRS} )

add_library( IlmImfCtl ${DO_SHARED}
  ImfCtlApplyTransforms.cpp
  ImfCtlCopyFunctionArg.cpp
)

target_link_libraries( IlmImfCtl IlmCtl IlmImf-2_2 Iex-2_2 IlmThread-2_2 Half )

install( FILES ImfCtlApplyTransforms.h DESTINATION include/OpenEXR )

export( TARGETS IlmImfCtl IlmCtl FILE "${PROJECT_BINARY_DIR}/CTLLibraryDepends.cmake" )

install( TARGETS IlmImfCtl DESTINATION lib )
