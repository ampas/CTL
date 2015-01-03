include_directories( "${CMAKE_CURRENT_SOURCE_DIR}" )

add_library( IlmCtl ${DO_SHARED}
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

target_link_libraries( IlmCtl ${IlmBase_LDFLAGS_OTHER} )
target_link_libraries( IlmCtl Iex IlmThread Half )

set_target_properties( IlmCtl PROPERTIES
  VERSION ${CTL_VERSION}
  SOVERSION ${CTL_VERSION}
)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set_target_properties(IlmCtl PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

install( FILES
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
 DESTINATION include/CTL )

install( TARGETS IlmCtl DESTINATION lib )
