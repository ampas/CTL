
if(win32)
  add_definitions( -DOPENEXR_DLL )
endif(win32)

include_directories( "${CMAKE_CURRENT_SOURCE_DIR}" )

add_library( ctldpx
 dpx.cc
 dpx_raw.cc
 dpx_read.cc
 dpx_util.cc
 dpx_write.cc
 dpx_bits.cc
 dpx_validate.cc
 dpx_rw.cc
)

