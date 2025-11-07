cmake_minimum_required(VERSION 3.15)
project(cosc522_lodi C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

file(GLOB COMMON_SRC CONFIGURE_DEPENDS
    src/shared/*.c
    src/shared/**/*.c
)
include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(lodi_client
    src/lodi-client/lodi_client.c
    ${COMMON_SRC}
)
target_link_libraries(lodi_client PRIVATE m)

add_executable(lodi_server
    src/lodi-server/lodi_server.c
    ${COMMON_SRC}
)
add_executable(pke_server
    src/pke-server/pke_server.c
    ${COMMON_SRC}
    src/pke-server/key_repository.c
    src/pke-server/key_repository.h
)
add_executable(tfa_client
    src/tfa-client/tfa_client.c
    ${COMMON_SRC}
)

add_executable(tfa_server
    src/tfa-server/tfa_server.c
    ${COMMON_SRC}
    src/tfa-server/registration_repository.c
    src/tfa-server/registration_repository.h
)

add_executable(rsa_test
    src/rsa-test/rsa_test.c
    ${COMMON_SRC}
)