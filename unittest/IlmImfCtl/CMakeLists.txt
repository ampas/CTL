
add_executable(IlmImfCtlTest 
    main.cpp
    testSourceDestination.cpp
    testTypes.cpp
)

include_directories( ${OpenEXR_INCLUDE_DIRS} )
link_directories( ${OpenEXR_LIBRARY_DIRS} )

include_directories( "${CMAKE_CURRENT_SOURCE_DIR}" 
                     "${PROJECT_SOURCE_DIR}/lib/IlmCtl" 
                     "${PROJECT_SOURCE_DIR}/lib/IlmCtlMath" 
                     "${PROJECT_SOURCE_DIR}/lib/IlmCtlSimd" 
                     "${PROJECT_SOURCE_DIR}/lib/IlmImfCtl" )
                     
target_link_libraries( IlmImfCtlTest IlmCtlSimd IlmCtlMath IlmCtl IlmImfCtl )
target_link_libraries( IlmImfCtlTest ${IlmBase_LIBRARIES} ${IlmBase_LDFLAGS_OTHER} )

add_test( IlmImfCtl IlmImfCtlTest )
add_dependencies(check IlmImfCtlTest)

file( 
    COPY
    function1.ctl
    function2.ctl
    function3.ctl
    DESTINATION
        ${CMAKE_CURRENT_BINARY_DIR}
)
