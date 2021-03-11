set(SOURCES
	CtlColorSpace.cpp
	CtlLookupTable.cpp
	CtlRbfInterpolator.cpp
)

add_library(IlmCtlMath ${SOURCES})

target_include_directories(IlmCtlMath
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(IlmCtlMath
	PUBLIC
		IlmCtl
	PUBLIC
		Imath::Imath
)

set_target_properties(IlmCtlMath
	PROPERTIES
		VERSION ${CTL_VERSION}
		SOVERSION ${CTL_VERSION}
)

install(FILES
	CtlColorSpace.h
	CtlLookupTable.h
	CtlRbfInterpolator.h
	CtlSparseMatrix.h
DESTINATION
	${CMAKE_INSTALL_INCLUDEDIR}/CTL
)

install(TARGETS IlmCtlMath DESTINATION ${CMAKE_INSTALL_LIBDIR})
