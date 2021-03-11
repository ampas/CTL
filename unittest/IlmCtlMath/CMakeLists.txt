set(SOURCES
	main.cpp
	testAffineRec.cpp
	testGaussRec.cpp
)

add_executable(IlmCtlMathTest ${SOURCES})

target_include_directories(IlmCtlMathTest
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(IlmCtlMathTest
	PRIVATE
		IlmCtlMath
		IlmCtl
)

add_test(IlmCtlMath IlmCtlMathTest)
