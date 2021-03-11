set(SOURCES
	 dpx.cc
	 dpx_raw.cc
	 dpx_read.cc
	 dpx_util.cc
	 dpx_write.cc
	 dpx_bits.cc
	 dpx_validate.cc
	 dpx_rw.cc
)
add_library(ctldpx STATIC ${SOURCES})
target_include_directories(ctldpx
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ctldpx
	PRIVATE
		Imath::Half
)
