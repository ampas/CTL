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

target_link_libraries (IlmCtl
    PRIVATE
        # For OpenEXR/Imath 3.x:
          $<$<TARGET_EXISTS:OpenEXR::OpenEXR>:OpenEXR::OpenEXR>
          $<$<TARGET_EXISTS:Imath::Imath>:Imath::Imath>
          $<$<TARGET_EXISTS:Imath::Half>:Imath::Half>
        # For OpenEXR 2.4/2.5:
          $<$<TARGET_EXISTS:OpenEXR::IlmImf>:OpenEXR::IlmImf>
          $<$<TARGET_EXISTS:IlmBase::Imath>:IlmBase::Imath>
          $<$<TARGET_EXISTS:IlmBase::Half>:IlmBase::Half>
          $<$<TARGET_EXISTS:IlmBase::IlmThread>:IlmBase::IlmThread>
          $<$<TARGET_EXISTS:IlmBase::Iex>:IlmBase::Iex>
    )

set_target_properties(IlmCtl
	PROPERTIES
		VERSION "${CTL_VERSION}"
		SOVERSION "${CTL_VERSION_MAJOR}.${CTL_VERSION_MINOR}"
)

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
DESTINATION
	${CMAKE_INSTALL_INCLUDEDIR}/CTL
)

install(TARGETS IlmCtl DESTINATION ${CMAKE_INSTALL_LIBDIR})
