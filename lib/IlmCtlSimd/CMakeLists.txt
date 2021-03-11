# Build the halfExpLogTable.h file
add_executable(halfExpLogTable halfExpLogTable.cpp)
target_link_libraries(halfExpLogTable
	PRIVATE
		Imath::Half
)
set(HALF_TABLE "${CMAKE_CURRENT_BINARY_DIR}/halfExpLogTable.h")
set(HALF_EXPFUNCS "${CMAKE_CURRENT_BINARY_DIR}/halfExpLog.h")

add_custom_command( OUTPUT "${HALF_TABLE}" "${HALF_EXPFUNCS}"
	COMMAND halfExpLogTable "${HALF_TABLE}" "${HALF_EXPFUNCS}"
	DEPENDS halfExpLogTable
)

set(SOURCES
	CtlSimdAddr.cpp
	CtlSimdFunctionCall.cpp
	CtlSimdHalfExpLog.cpp
	CtlSimdInst.cpp
	CtlSimdInterpreter.cpp
	CtlSimdLContext.cpp
	CtlSimdModule.cpp
	CtlSimdReg.cpp
	CtlSimdStdLibAssert.cpp
	CtlSimdStdLibColorSpace.cpp
	CtlSimdStdLibInterpolator.cpp
	CtlSimdStdLibLimits.cpp
	CtlSimdStdLibLookupTable.cpp
	CtlSimdStdLibMath.cpp
	CtlSimdStdLibPrint.cpp
	CtlSimdStdLibrary.cpp
	CtlSimdStdTypes.cpp
	CtlSimdSyntaxTree.cpp
	CtlSimdType.cpp
	CtlSimdXContext.cpp
	${HALF_TABLE}
	${HALF_EXPFUNCS}
)
add_library(IlmCtlSimd ${SOURCES})

target_include_directories(IlmCtlSimd
	PRIVATE
		${CMAKE_CURRENT_BINARY_DIR}
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(IlmCtlSimd
	PUBLIC
		IlmCtlMath
		IlmCtl
)

set_target_properties(IlmCtlSimd
	PROPERTIES
		VERSION ${CTL_VERSION}
		SOVERSION ${CTL_VERSION}
)

install(FILES CtlSimdInterpreter.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/CTL)

install(TARGETS IlmCtlSimd DESTINATION ${CMAKE_INSTALL_LIBDIR})
