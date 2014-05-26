

add_test(
    NAME ctlrender
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ./test.sh $<TARGET_FILE:ctlrender>
)

#add_dependencies(check ctlrender)
