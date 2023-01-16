
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

#target_link_libraries( exrdpx IlmCtlSimd IlmCtlMath IlmCtl IlmImfCtl )
target_link_libraries (exrdpx
    PRIVATE
		IlmCtlSimd 
		IlmCtlMath 
		IlmCtl 
		IlmImfCtl
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
#target_link_libraries( exrdpx ${IlmBase_LIBRARIES} ${IlmBase_LDFLAGS_OTHER} )
#target_link_libraries( exrdpx ${OpenEXR_LIBRARIES} ${OpenEXR_LDFLAGS_OTHER} )

install( TARGETS exrdpx DESTINATION ${CMAKE_INSTALL_BINDIR} )

install( FILES transform_DPX_EXR.ctl transform_EXR_DPX.ctl DESTINATION ${CMAKE_INSTALL_DATADIR}/CTL )


