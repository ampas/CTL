set(SOURCES
	main.cc
	transform.cc
	usage.cc
	aces_file.cc
	dpx_file.cc
	exr_file.cc
	tiff_file.cc
	format.cc
	compression.cc
)
add_executable(ctlrender ${SOURCES})
target_include_directories(ctlrender
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(ctlrender
	PRIVATE
		IlmCtlSimd
		IlmCtlMath
		IlmCtl
		ctldpx
	    # For OpenEXR/Imath 3.x:
          $<$<TARGET_EXISTS:OpenEXR::OpenEXR>:OpenEXR::OpenEXR>
        # For OpenEXR 2.4/2.5:
          $<$<TARGET_EXISTS:OpenEXR::IlmImf>:OpenEXR::IlmImf>
)

if( OpenEXR_FOUND )
	target_compile_definitions(ctlrender
	PRIVATE
		-DHAVE_OPENEXR=1
	)
else()
	message( STATUS "OPENEXR not found, ctlrender will not support EXR files" )
endif()

find_package(TIFF)
if(TARGET TIFF::TIFF)
	message( STATUS "found TIFF, TIFF_LIBRARIES : ${TIFF_LIBRARIES}" )
	target_compile_definitions(ctlrender
		PRIVATE
			-DHAVE_LIBTIFF=1
	)
	target_link_libraries(ctlrender
		PRIVATE
			TIFF::TIFF
	)
else()
	message( STATUS "TIFF not found, ctlrender will not support TIF files" )
endif()

find_package(AcesContainer)
if (TARGET AcesContainer::AcesContainer)
	target_compile_definitions(ctlrender
		PRIVATE
			-DHAVE_ACESFILE=1
	)
	target_link_libraries(ctlrender
		PRIVATE
			AcesContainer::AcesContainer
	)
endif()

install(TARGETS ctlrender DESTINATION ${CMAKE_INSTALL_BINDDIR})
