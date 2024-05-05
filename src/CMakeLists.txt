cmake_minimum_required(VERSION 3.10)

set(LIBRARY_NAME bkrpclient)

include_directories ("${CMAKE_BINARY_DIR}/src")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(samba_HEADERS
    idl_types.h
    misc.idl
    security.idl
)

# Headers
set(bkrp_client_HEADERS
    bkrp.acf
    bkrp.idl
    bkrp_client.h
)

add_custom_target(ms-bkrp-idl SOURCES
    ${bkrp_client_HEADERS}
    ${samba_HEADERS}
)

# Sources
set(bkrp_client_SOURCES
    bkrp_client.c
)

set(bkrp_client_SOURCES ${bkrp_client_SOURCES} ${bkrp_client_HEADERS})

# Generated files
set(bkrp_client_GEN_HEADERS
    bkrp.h
    ndr_bkrp.h
    ndr_bkrp_c.h
)

set(bkrp_client_GEN_SOURCES
    ndr_bkrp_c.c
    ndr_bkrp.c
)

# Generated files should be excluded from the build
#set_source_files_properties(${bkrp_client_GEN_SOURCES} ${bkrp_client_GEN_HEADERS} ${bkrp_local_client_GEN_SOURCES} PROPERTIES GENERATED TRUE)

# IDL compilation rule
set(IDL_EXECUTABLE ${IDL_COMPILER})

add_custom_target(
    generated_files
    ALL
    COMMAND ${IDL_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/bkrp.idl --client --header --ndr-parser --outputdir=${CMAKE_CURRENT_SOURCE_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM
)

file(TO_NATIVE_PATH "/" CMAKE_PATH_SEPARATOR)

foreach(FILE ${bkrp_client_GEN_SOURCES})
    string(CONCAT SOURCE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_PATH_SEPARATOR} ${FILE})
    message("Generated file: ${SOURCE}")
    set(bkrp_local_client_GEN_SOURCES ${bkrp_local_client_GEN_SOURCES} ${SOURCE})
endforeach()

# Dot in include path is intentional to avoid escaping additional slash
add_custom_target(
    remove_include
    ALL
    COMMAND sed -i s/\#include\ \<gen_ndr.ndr\_security\.h\>//g ndr_bkrp.c
    DEPENDS generated_files
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM
)

# Libraries
find_package(PkgConfig REQUIRED)
pkg_check_modules(Ndr REQUIRED IMPORTED_TARGET ndr)
pkg_check_modules(Tevent REQUIRED IMPORTED_TARGET tevent)
pkg_check_modules(DCERPC REQUIRED IMPORTED_TARGET dcerpc)
pkg_check_modules(SambaCredentials REQUIRED IMPORTED_TARGET samba-credentials)
pkg_check_modules(SambaHostconfig REQUIRED IMPORTED_TARGET samba-hostconfig)
pkg_check_modules(SambaUtil REQUIRED IMPORTED_TARGET samba-util)
pkg_check_modules(Krb5GssAPI REQUIRED IMPORTED_TARGET krb5-gssapi)
find_library(SAMBA4_SECURITY
    NAMES
        "libsamba-security-samba4.so"
    HINTS
        "/usr/lib64/samba/"
        "/usr/lib/"
)
find_library(TEVENT_UTIL
    NAMES
        "libtevent-util.so"
    HINTS
        "/usr/lib64/samba/"
        "/usr/lib/"
)
find_library(SAMBA4_NDR
    NAMES
        "libndr-samba4.so"
    HINTS
        "/usr/lib64/samba/"
        "/usr/lib/"
)

# Target
add_library(${LIBRARY_NAME} SHARED ${bkrp_client_SOURCES} ${bkrp_client_GEN_HEADERS} ${bkrp_local_client_GEN_SOURCES})

# Link libraries
target_link_libraries(${LIBRARY_NAME} PUBLIC
    PkgConfig::Ndr
    PkgConfig::Tevent
    PkgConfig::DCERPC
    PkgConfig::SambaCredentials
    PkgConfig::SambaHostconfig
    ${SAMBA4_SECURITY}
    ${TEVENT_UTIL}
    ${SAMBA4_NDR}
    PkgConfig::SambaUtil
    PkgConfig::Krb5GssAPI
)

add_dependencies(${LIBRARY_NAME}
    generated_files
    remove_include
)

# Installation
install(TARGETS ${LIBRARY_NAME} DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${bkrp_client_HEADERS} ${bkrp_client_GEN_HEADERS} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/bkrp")
