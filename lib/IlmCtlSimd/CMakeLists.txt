include_directories( "${CMAKE_CURRENT_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}/lib/IlmCtl" "${PROJECT_SOURCE_DIR}/lib/IlmCtlMath" )

if(WIN32)
  add_definitions( -DOPENEXR_DLL )
endif(WIN32)


add_executable( halfExpLogTable halfExpLogTable.cpp )
target_link_libraries( halfExpLogTable ${IlmBase_LDFLAGS_OTHER} )
target_link_libraries( halfExpLogTable Iex-2_2 Imath-2_2 IexMath-2_2 IlmThread-2_2 Half )

add_custom_command( OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/halfExpLogTable.h"
                    COMMAND halfExpLogTable "${CMAKE_CURRENT_BINARY_DIR}/halfExpLogTable.h" "${CMAKE_CURRENT_BINARY_DIR}/halfExpLog.h"
                    DEPENDS halfExpLogTable )

include_directories( "${CMAKE_CURRENT_BINARY_DIR}" )

add_library( IlmCtlSimd ${DO_SHARED}
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
	"${CMAKE_CURRENT_BINARY_DIR}/halfExpLogTable.h"
)

target_link_libraries( IlmCtlSimd ${IlmBase_LDFLAGS_OTHER} )
target_link_libraries( IlmCtlSimd IlmCtlMath IlmCtl )

set_target_properties( IlmCtlSimd PROPERTIES
  VERSION ${CTL_VERSION}
  SOVERSION ${CTL_VERSION}
)

install( FILES CtlSimdInterpreter.h DESTINATION include/CTL )

install( TARGETS IlmCtlSimd DESTINATION lib )
