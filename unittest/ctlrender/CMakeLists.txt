

add_test(
    NAME ctlrender
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ./test.sh
)

#add_dependencies(check ctlrender)
