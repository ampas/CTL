
include_directories( ${OpenEXR_INCLUDE_DIRS} )
link_directories( ${OpenEXR_LIBRARY_DIRS} )

include_directories( "${CMAKE_CURRENT_SOURCE_DIR}" 
                     "${PROJECT_SOURCE_DIR}/lib/IlmCtl" 
                     "${PROJECT_SOURCE_DIR}/lib/IlmCtlMath" 
                     "${PROJECT_SOURCE_DIR}/lib/IlmCtlSimd" 
                     "${PROJECT_SOURCE_DIR}/lib/IlmImfCtl" )

add_executable( exr_ctl_exr
	applyCtl.cpp
	applyCtl.h
	exrCtlExr.cpp
	exrCtlExr.h
	main.cpp
)

target_link_libraries( exr_ctl_exr IlmCtlSimd IlmCtlMath IlmCtl IlmImfCtl )
target_link_libraries( exr_ctl_exr ${IlmBase_LIBRARIES} ${IlmBase_LDFLAGS_OTHER} )
target_link_libraries( exr_ctl_exr ${OpenEXR_LIBRARIES} ${OpenEXR_LDFLAGS_OTHER} )

install( TARGETS exr_ctl_exr DESTINATION ${INSTALL_BIN_DIR} )

install( FILES change_saturation.ctl DESTINATION ${INSTALL_LIB_DIR}/CTL )

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set_target_properties(exr_ctl_exr PROPERTIES INSTALL_RPATH "@loader_path/../lib")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
