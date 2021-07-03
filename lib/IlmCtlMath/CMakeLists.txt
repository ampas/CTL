
if(WIN32)
  add_definitions( -DOPENEXR_DLL )
endif(WIN32)


include_directories( "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/lib/IlmCtl" )

add_library( IlmCtlMath ${DO_SHARED}
  CtlColorSpace.cpp
  CtlLookupTable.cpp
  CtlRbfInterpolator.cpp
)

target_link_libraries( IlmCtlMath IlmCtl )

set_target_properties( IlmCtlMath PROPERTIES
  VERSION ${CTL_VERSION}
  SOVERSION ${CTL_VERSION}
)

install( FILES
  CtlColorSpace.h
  CtlLookupTable.h
  CtlRbfInterpolator.h
  CtlSparseMatrix.h
 DESTINATION include/CTL )

install( TARGETS IlmCtlMath DESTINATION lib )

# TODO Add Imath and Iex libadd dependency and version info
