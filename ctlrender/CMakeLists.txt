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

#set(AcesContainer_DIR "/usr/local/lib/CMake/AcesContainer")
#25 0.685 CMake Warning at ctlrender/CMakeLists.txt:53 (find_package):
#25 0.685   By not providing "FindAcesContainer.cmake" in CMAKE_MODULE_PATH this
#25 0.685   project has asked CMake to find a package configuration file provided by
#25 0.685   "AcesContainer", but CMake did not find one.
#25 0.685 
#25 0.685   Could not find a package configuration file provided by "AcesContainer"
#25 0.685   with any of the following names:
#25 0.685 
#25 0.685     AcesContainerConfig.cmake
#25 0.685     acescontainer-config.cmake
#25 0.685 
#25 0.685   Add the installation prefix of "AcesContainer" to CMAKE_PREFIX_PATH or set
#25 0.685   "AcesContainer_DIR" to a directory containing one of the above files.  If
#25 0.685   "AcesContainer" provides a separate development package or SDK, be sure it
#25 0.685   has been installed.
#set(CMAKE_MODULE_PATH "/usr/local/lib/CMake/AcesContainer;${CMAKE_MODULE_PATH}")
#set(AcesContainer_DIR "/usr/local/lib/CMake/AcesContainer")
#set(CMAKE_PREFIX_PATH "/usr/local/lib/CMake/AcesContainer;C:/Program Files (x86)/AcesContainer/CMake/;${CMAKE_PREFIX_PATH}")

find_package(AcesContainer)
if( AcesContainer_FOUND )
	message( STATUS "find_package(AcesContainer) - AcesContainer_FOUND is true")
	message( STATUS "	AcesContainer_VERSION : ${AcesContainer_VERSION}" )
	message( STATUS "	AcesContainer_INCLUDE_DIRS : ${AcesContainer_INCLUDE_DIRS}" )
	message( STATUS "	AcesContainer_LIBRARIES : ${AcesContainer_LIBRARIES}" )
	message( STATUS "	AcesContainer_INCLUDE_DIR : ${AcesContainer_INCLUDE_DIR}" )
	message( STATUS "	AcesContainer_LIBRARY : ${AcesContainer_LIBRARY}" )
	
else()
	message( STATUS "DEBUG: find_package(AcesContainer) AcesContainer_FOUND is false")
endif()

if (TARGET AcesContainer::AcesContainer)
	message( STATUS "find_package(AcesContainer) - TARGET AcesContainer::AcesContainer is true:" )
	message( STATUS "	AcesContainer_VERSION : ${AcesContainer_VERSION}" )
	message( STATUS "	AcesContainer_INCLUDE_DIRS : ${AcesContainer_INCLUDE_DIRS}" )
	message( STATUS "	AcesContainer_LIBRARIES : ${AcesContainer_LIBRARIES}" )
	message( STATUS "	AcesContainer_INCLUDE_DIR : ${AcesContainer_INCLUDE_DIR}" )
	message( STATUS "	AcesContainer_LIBRARY : ${AcesContainer_LIBRARY}" )
else()
	message( STATUS "AcesContainer not found - TARGET AcesContainer::AcesContainer is false")
endif()

if( AcesContainer_FOUND OR TARGET AcesContainer::AcesContainer)
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
