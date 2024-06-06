set(SOURCES
  ImfCtlApplyTransforms.cpp
  ImfCtlCopyFunctionArg.cpp
)

add_library(IlmImfCtl ${SOURCES})

target_include_directories(IlmImfCtl
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
)

#target_link_libraries(IlmImfCtl
#	PUBLIC
#		IlmCtl
#	PRIVATE
#		OpenEXR::OpenEXR
#		OpenEXR::Iex
#		OpenEXR::IlmThread
#		Imath::Half
#)
target_link_libraries (IlmImfCtl
	PUBLIC
		IlmCtl
  PUBLIC
        # For OpenEXR/Imath 3.x:
          $<$<TARGET_EXISTS:OpenEXR::OpenEXR>:OpenEXR::OpenEXR>
#          $<$<TARGET_EXISTS:Imath::Imath>:Imath::Imath>
          $<$<TARGET_EXISTS:Imath::Half>:Imath::Half>
        # For OpenEXR 2.4/2.5:
          $<$<TARGET_EXISTS:OpenEXR::IlmImf>:OpenEXR::IlmImf>
 #         $<$<TARGET_EXISTS:IlmBase::Imath>:IlmBase::Imath>
          $<$<TARGET_EXISTS:IlmBase::Half>:IlmBase::Half>
          $<$<TARGET_EXISTS:IlmBase::IlmThread>:IlmBase::IlmThread>
          $<$<TARGET_EXISTS:IlmBase::Iex>:IlmBase::Iex>
    )

set_target_properties(IlmImfCtl
       PROPERTIES
               VERSION ${CTL_VERSION}
               SOVERSION ${CTL_VERSION}
)

install(FILES
	ImfCtlApplyTransforms.h
DESTINATION
	${CMAKE_INSTALL_INCLUDEDIR}/OpenEXR
)

install(TARGETS IlmImfCtl DESTINATION ${CMAKE_INSTALL_LIBDIR})
