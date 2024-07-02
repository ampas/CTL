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
		$<$<TARGET_EXISTS:Imath::Imath>:Imath::Imath>
		$<$<TARGET_EXISTS:Imath::Half>:Imath::Half>
	  # For OpenEXR 2.4/2.5:
		$<$<TARGET_EXISTS:OpenEXR::IlmImf>:OpenEXR::IlmImf>
		$<$<TARGET_EXISTS:IlmBase::Imath>:IlmBase::Imath>
		$<$<TARGET_EXISTS:IlmBase::Half>:IlmBase::Half>
		$<$<TARGET_EXISTS:IlmBase::IlmThread>:IlmBase::IlmThread>
		$<$<TARGET_EXISTS:IlmBase::Iex>:IlmBase::Iex>
	  # ZLIB for OpenEXR 2.x:
	  	$<$<TARGET_EXISTS:ZLIB::ZLIB>:ZLIB::ZLIB>
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
if( AcesContainer_FOUND )
	message( STATUS "find_package(AcesContainer) - AcesContainer_FOUND is true")	
else()
	message( STATUS "find_package(AcesContainer) - AcesContainer_FOUND is false")
endif()

if (TARGET AcesContainer::AcesContainer)
	message( STATUS "find_package(AcesContainer) - TARGET AcesContainer::AcesContainer is true:" )
else()
	message( STATUS "find_package(AcesContainer) - TARGET AcesContainer::AcesContainer is false")
endif()

if( AcesContainer_FOUND OR TARGET AcesContainer::AcesContainer)

	message( STATUS "	AcesContainer_VERSION : ${AcesContainer_VERSION}" )
	message( STATUS "	AcesContainer_INCLUDE_DIRS : ${AcesContainer_INCLUDE_DIRS}" )
	message( STATUS "	AcesContainer_LIBRARIES : ${AcesContainer_LIBRARIES}" )
	message( STATUS "	AcesContainer_INCLUDE_DIR : ${AcesContainer_INCLUDE_DIR}" )
	message( STATUS "	AcesContainer_LIBRARY : ${AcesContainer_LIBRARY}" )

	target_compile_definitions(ctlrender
		PRIVATE
			-DHAVE_ACESFILE=1
	)
	target_link_libraries(ctlrender
		PRIVATE
			AcesContainer::AcesContainer
	)
	message( STATUS "AcesContainer found")
	message( STATUS "  ctlrender will support writing AcesContainer files" )
else()
	message( STATUS "AcesContainer not found - TARGET AcesContainer::AcesContainer is false and AcesContainer_FOUND is false")
	message( STATUS "  ctlrender will not support writing AcesContainer files" )
endif()

install(TARGETS ctlrender DESTINATION ${CMAKE_INSTALL_BINDIR})
