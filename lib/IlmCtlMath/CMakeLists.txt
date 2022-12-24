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

if(IlmBase_FOUND)
	target_link_libraries(IlmCtlMath
	PUBLIC
		IlmCtl
	PUBLIC
		# For OpenEXR 2.4/2.5:
        $<$<TARGET_EXISTS:OpenEXR::IlmImf>:OpenEXR::IlmImf>
        $<$<TARGET_EXISTS:IlmBase::Imath>:IlmBase::Imath>
	)	
else()
	target_link_libraries(IlmCtlMath
	PUBLIC
		IlmCtl
	PUBLIC
		# For OpenEXR/Imath 3.x:
        $<$<TARGET_EXISTS:OpenEXR::OpenEXR>:OpenEXR::OpenEXR>
        $<$<TARGET_EXISTS:Imath::Imath>:Imath::Imath>
	)
endif()

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
