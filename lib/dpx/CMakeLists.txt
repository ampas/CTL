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
	PUBLIC
	# For OpenEXR/Imath 3.x:
	$<$<TARGET_EXISTS:Imath::Imath>:Imath::Imath>
	# For OpenEXR 2.4/2.5:
	$<$<TARGET_EXISTS:IlmBase::Imath>:IlmBase::Imath>
)
#target_link_libraries (ctldpx
#    PRIVATE
#        # For OpenEXR/Imath 3.x:
#          $<$<TARGET_EXISTS:OpenEXR::OpenEXR>:OpenEXR::OpenEXR>
#          $<$<TARGET_EXISTS:Imath::Imath>:Imath::Imath>
#          $<$<TARGET_EXISTS:Imath::Half>:Imath::Half>
#        # For OpenEXR 2.4/2.5:
#          $<$<TARGET_EXISTS:OpenEXR::IlmImf>:OpenEXR::IlmImf>
#          $<$<TARGET_EXISTS:IlmBase::Imath>:IlmBase::Imath>
#          $<$<TARGET_EXISTS:IlmBase::Half>:IlmBase::Half>
#          $<$<TARGET_EXISTS:IlmBase::IlmThread>:IlmBase::IlmThread>
#          $<$<TARGET_EXISTS:IlmBase::Iex>:IlmBase::Iex>
#    )