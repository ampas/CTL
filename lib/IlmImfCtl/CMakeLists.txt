set(SOURCES
  ImfCtlApplyTransforms.cpp
  ImfCtlCopyFunctionArg.cpp
)

add_library(IlmImfCtl ${SOURCES})

target_include_directories(IlmImfCtl
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(IlmImfCtl
	PUBLIC
		IlmCtl
	PRIVATE
		OpenEXR::OpenEXR
		OpenEXR::Iex
		OpenEXR::IlmThread
		Imath::Half
)

install(FILES
	ImfCtlApplyTransforms.h
DESTINATION
	${CMAKE_INSTALL_LIBDIR}/OpenEXR
)

install(TARGETS IlmImfCtl DESTINATION ${CMAKE_INSTALL_LIBDIR})
