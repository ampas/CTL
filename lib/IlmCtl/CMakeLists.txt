set(SOURCES
	CtlAddr.cpp
	CtlAlign.cpp
	CtlExc.cpp
	CtlFunctionCall.cpp
	CtlInterpreter.cpp
	CtlLContext.cpp
	CtlLex.cpp
	CtlMessage.cpp
	CtlModule.cpp
	CtlModuleSet.cpp
	CtlParser.cpp
	CtlRcPtr.cpp
	CtlSymbolTable.cpp
	CtlSyntaxTree.cpp
	CtlTokens.cpp
	CtlType.cpp
	CtlTypeStorage.cpp
	CtlStdType.cpp
)

add_library(IlmCtl ${SOURCES})

target_include_directories(IlmCtl
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(IlmCtl
	PUBLIC
		OpenEXR::Iex
		OpenEXR::IlmThread
		Imath::Half
)

set_target_properties(IlmCtl
	PROPERTIES
		VERSION ${CTL_VERSION}
		SOVERSION ${CTL_VERSION}
)

install(FILES
	CtlAddr.h
	CtlAlign.h
	CtlErrors.h
	CtlExc.h
	CtlFunctionCall.h
	CtlInterpreter.h
	CtlLContext.h
	CtlMessage.h
	CtlModule.h
	CtlRcPtr.h
	CtlReadWriteAccess.h
	CtlSymbolTable.h
	CtlSyntaxTree.h
	CtlTokens.h
	CtlType.h
	CtlTypeStorage.h
	CtlStdType.h
	CtlVersion.h
DESTINATION
	${CMAKE_INSTALL_INCLUDEDIR}/CTL
)

install(TARGETS IlmCtl DESTINATION ${CMAKE_INSTALL_LIBDIR})
