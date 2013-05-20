
add_executable(IlmCtlTest 
    main.cpp
    testCppCall.cpp
    testEndOfLine.cpp
    testExamples.cpp
    testHugeInit.cpp
    testParser.cpp
    testVarying.cpp
    testVaryingLookup.cpp
    testVaryingReturn.cpp
)

include_directories( "${CMAKE_CURRENT_SOURCE_DIR}" 
                     "${PROJECT_SOURCE_DIR}/lib/IlmCtl"  
                     "${PROJECT_SOURCE_DIR}/lib/IlmCtlSimd" )
                     
target_link_libraries( IlmCtlTest IlmCtlSimd IlmCtlMath IlmCtl )
target_link_libraries( IlmCtlTest ${IlmBase_LIBRARIES} ${IlmBase_LDFLAGS_OTHER} )

add_test( IlmCtl IlmCtlTest )
add_dependencies(check IlmCtlTest)

file( 
    COPY
        common.ctl
        example.ctl
        testArray.ctl
        testCast.ctl
        testComments.ctl
        testCppCall.ctl
        testCtlVersion.ctl
        test.ctl
        testDefaults.ctl
        testEmpty.ctl
        testExamples.ctl
        testExamplesNamespace.ctl
        testExpr.ctl
        testFunc.ctl
        testHugeInit.ctl
        testInterpolator.ctl
        testLiterals.ctl
        testLookupTables.ctl
        testLoops.ctl
        testName2.ctl
        testName.ctl
        testNameSpace2.ctl
        testNameSpace.ctl
        testNoName.ctl
        testParse.ctl
        testScope2.ctl
        testScope.ctl
        testStdLibrary.ctl
        testStruct.ctl
        testTypes.ctl
        testVarying.ctl
        testVaryingLookup.ctl
        testVaryingReturn.ctl
        testVSArrays.ctl
    DESTINATION
        ${CMAKE_CURRENT_BINARY_DIR}
)
