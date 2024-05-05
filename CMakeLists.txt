cmake_minimum_required(VERSION 3.12)

project(libbkrp)

execute_process(
    COMMAND
    bash -c "grep 'Version:' .gear/libbkrp.spec | awk -F ':' '{ print $2 }' | tr -d [:space:]"
    OUTPUT_VARIABLE LIB_BKRP_GEAR_VERSION
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

string(REPLACE "." ";" GEAR_VERSION_LIST ${LIB_BKRP_GEAR_VERSION})
list(GET GEAR_VERSION_LIST 0 LIB_BKRP_VERSION_MAJOR)
list(GET GEAR_VERSION_LIST 1 LIB_BKRP_VERSION_MINOR)
list(GET GEAR_VERSION_LIST 2 LIB_BKRP_VERSION_PATCH)

if(UNIX)
  include(GNUInstallDirs)
else()
    message(FATAL_ERROR "System not UNIX - not implemented yet")
endif()

set(BKRP_IN_BUILD_TREE TRUE)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

configure_file(
    "${CMAKE_MODULE_PATH}/BkrpConfig.cmake.in"
    "${CMAKE_BINARY_DIR}/BkrpConfig.cmake"
    IMMEDIATE @ONLY
)
install(FILES ${CMAKE_BINARY_DIR}/BkrpConfig.cmake DESTINATION "${CMAKE_INSTALL_LIBDIR}/bkrp")

configure_file(
    "${CMAKE_MODULE_PATH}/BkrpBuildTreeSettings.cmake.in"
    "${CMAKE_BINARY_DIR}/BkrpBuildTreeSettings.cmake"
    IMMEDIATE @ONLY
)

export(PACKAGE Bkrp)

include(FindIDL)

add_subdirectory(examples)

add_subdirectory(src)

option(BKRP_BUILD_TESTS "Build bkrp tests." ON)
option(BKRP_BUILD_EXAMPLES "Build bkrp examples." ON)

enable_testing()
add_subdirectory(tests)
