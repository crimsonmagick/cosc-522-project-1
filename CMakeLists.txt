cmake_minimum_required(VERSION 3.15)
project(cosc522_lodi C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

file(GLOB COMMON_SRC CONFIGURE_DEPENDS src/shared/**/*.c)
include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(echo_client
    src/echo-example/udp_echo_client.c
    ${COMMON_SRC}
)

add_executable(echo_server
    src/echo-example/udp_echo_server.c
    ${COMMON_SRC}
)

add_executable(lodi_client
    src/lodi-client/lodi_client.c
    ${COMMON_SRC}
    include/messaging/utils.h
    src/shared/messaging/utils.c
)
add_executable(lodi_server
    src/lodi-server/lodi_server.c
    ${COMMON_SRC}
    include/messaging/utils.h
    src/shared/messaging/utils.c
)
add_executable(pke_server
    src/pke-server/pke_server.c
    ${COMMON_SRC}
    include/messaging/utils.h
    src/shared/messaging/utils.c
)
add_executable(tfa_client
    src/tfa-client/tfa_client.c
    ${COMMON_SRC}
    include/messaging/utils.h
    src/shared/messaging/utils.c
)

add_executable(tfa_server
    src/tfa-server/tfa_server.c
    ${COMMON_SRC}
    include/messaging/utils.h
    src/shared/messaging/utils.c
)