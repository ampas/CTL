include_directories( "${CMAKE_CURRENT_SOURCE_DIR}" )

add_library( ctladx
 adx.cc
 adx_raw.cc
 adx_read.cc
 adx_util.cc
 adx_write.cc
 adx_bits.cc
 adx_validate.cc
 adx_rw.cc
)

