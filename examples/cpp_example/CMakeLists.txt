find_package(Bkrp)

set(EXAMPLE_NAME
    bkrp_example
)

set(SOURCES
    main.cpp
)

add_executable(${EXAMPLE_NAME} ${SOURCES})
target_include_directories(${EXAMPLE_NAME} PRIVATE ${BKRP_INCLUDE_DIR})
target_link_directories(${EXAMPLE_NAME} PRIVATE ${BKRP_LIBRARY_DIR})
target_link_libraries(${EXAMPLE_NAME} PRIVATE ${BKRP_LIBRARY})
