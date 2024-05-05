find_package(Bkrp)

set(TEST_NAME
    bkrp_test
)

add_executable(${TEST_NAME} bkrp_test.c)

add_test(NAME bkrp COMMAND ${TEST_NAME})
target_include_directories(${TEST_NAME} PRIVATE ${BKRP_INCLUDE_DIR})
target_link_directories(${TEST_NAME} PRIVATE ${BKRP_LIBRARY_DIR})
target_link_libraries(${TEST_NAME} PRIVATE ${BKRP_LIBRARY})
