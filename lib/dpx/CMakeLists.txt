
if(WIN32)
  add_definitions( -DOPENEXR_DLL )
endif(WIN32)

include_directories( "${CMAKE_CURRENT_SOURCE_DIR}" )

add_library( ctldpx ${DO_SHARED}
 dpx.cc
 dpx_raw.cc
 dpx_read.cc
 dpx_util.cc
 dpx_write.cc
 dpx_bits.cc
 dpx_validate.cc
 dpx_rw.cc
)

add_definitions( -DIMATH_DLL -DOPENEXR_DLL )
target_link_libraries( ctldpx ${OPENEXR_LIBRARIES} ${IlmBase_LIBRARIES} )
