

add_test(
    NAME ctlrender
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ./test.sh $<TARGET_FILE:ctlrender>
)

set_tests_properties(ctlrender PROPERTIES ENVIRONMENT "PATH=${CMAKE_BINARY_DIR}/ctlrender:$ENV{PATH}")

add_dependencies(check ctlrender)
