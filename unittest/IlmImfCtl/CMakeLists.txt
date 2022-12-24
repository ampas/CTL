set(SOURCES
    main.cpp
    testSourceDestination.cpp
    testTypes.cpp
)

add_executable(IlmImfCtlTest ${SOURCES})

target_include_directories(IlmImfCtlTest
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries( IlmImfCtlTest
	PRIVATE
		IlmImfCtl
		IlmCtlSimd
		IlmCtlMath
		IlmCtl
)

add_test( IlmImfCtl IlmImfCtlTest )

file(COPY
    function1.ctl
    function2.ctl
    function3.ctl
DESTINATION
	${CMAKE_CURRENT_BINARY_DIR}
)
