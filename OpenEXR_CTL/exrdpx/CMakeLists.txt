
include_directories( ${OpenEXR_INCLUDE_DIRS} )
link_directories( ${OpenEXR_LIBRARY_DIRS} )

include_directories( "${CMAKE_CURRENT_SOURCE_DIR}" 
                     "${PROJECT_SOURCE_DIR}/lib/IlmCtl" 
                     "${PROJECT_SOURCE_DIR}/lib/IlmCtlMath" 
                     "${PROJECT_SOURCE_DIR}/lib/IlmCtlSimd" 
                     "${PROJECT_SOURCE_DIR}/lib/IlmImfCtl" )

add_executable( exrdpx
    applyCtl.cpp
	dpxHeader.cpp
	dpxToExr.cpp
	exrToDpx.cpp
	main.cpp
)

target_link_libraries( exrdpx IlmCtlSimd IlmCtlMath IlmCtl IlmImfCtl )
target_link_libraries( exrdpx ${IlmBase_LIBRARIES} ${IlmBase_LDFLAGS_OTHER} )
target_link_libraries( exrdpx ${OpenEXR_LIBRARIES} ${OpenEXR_LDFLAGS_OTHER} )

install( TARGETS exrdpx DESTINATION ${INSTALL_BIN_DIR} )

install( FILES transform_DPX_EXR.ctl transform_EXR_DPX.ctl DESTINATION ${INSTALL_LIB_DIR}/CTL )

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set_target_properties(exrdpx PROPERTIES INSTALL_RPATH "@loader_path/../lib")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
