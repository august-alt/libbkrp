cmake_minimum_required(VERSION 3.10)

set(LIBRARY_NAME bkrpclient)

include_directories ("${CMAKE_BINARY_DIR}/src")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Headers
set(bkrp_client_HEADERS
    bkrp_client.h
    bkrp_client_error.h
    bkrp.idl
    bkrp.acf
)

# Sources
set(bkrp_client_SOURCES
    bkrp_client.c
)

set(bkrp_client_SOURCES ${bkrp_client_SOURCES} ${bkrp_client_HEADERS})

# Generated files
set(bkrp_client_GEN_HEADERS
    bkrp.h
)

set(bkrp_client_GEN_SOURCES
    bkrp_cstub.c
)

# Generated files should be excluded from the build
set_source_files_properties(${bkrp_client_GEN_SOURCES} ${bkrp_client_GEN_HEADERS} ${bkrp_local_client_GEN_SOURCES} PROPERTIES GENERATED TRUE)

# IDL compilation rule
set(IDL_EXECUTABLE ${IDL_COMPILER})

add_custom_target(
    generated_files
    ALL
    COMMAND ${IDL_EXECUTABLE} -keep c_source ${CMAKE_CURRENT_SOURCE_DIR}/bkrp.idl
    DEPENDS ${bkrp_client_HEADERS}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM
)

file(TO_NATIVE_PATH "/" CMAKE_PATH_SEPARATOR)

foreach(FILE ${bkrp_client_GEN_SOURCES})
    string(CONCAT SOURCE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_PATH_SEPARATOR} ${FILE})
    message("Generated file: ${SOURCE}")
    set(bkrp_local_client_GEN_SOURCES ${bkrp_local_client_GEN_SOURCES} ${SOURCE})
endforeach()

# Libraries
find_library(dcerpc dcerpc)
find_library(uuid uuid)
find_library(gssapi_krb5 gssapi_krb5)
find_library(krb5 krb5)

# Target
add_library(${LIBRARY_NAME} SHARED ${bkrp_client_SOURCES} ${bkrp_local_client_GEN_SOURCES})

# Link libraries
target_link_libraries(${LIBRARY_NAME}
    ${dcerpc}
    ${uuid}
    ${gssapi_krb5}
    ${krb5}
)

add_dependencies(${LIBRARY_NAME}
    generated_files
)

# Installation
install(TARGETS ${LIBRARY_NAME} DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${bkrp_client_HEADERS} ${bkrp_client_GEN_HEADERS} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/bkrp")
