cmake_minimum_required(VERSION 3.15)
project(cosc522_lodi C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

file(GLOB COMMON_SRC CONFIGURE_DEPENDS common/*.c)

add_executable(echo_client
    echo-example/udp_echo_client.c
    ${COMMON_SRC}
)
target_include_directories(echo_client PRIVATE common)

add_executable(echo_server
    echo-example/udp_echo_server.c
    ${COMMON_SRC}
)
target_include_directories(echo_server PRIVATE common)


add_executable(lodi_client
    lodi-client/lodi_client.c
    ${COMMON_SRC}
    common/pke_messaging.c
    common/pke_messaging.h
    common/lodi_messaging.c
    common/lodi_messaging.h
)
target_include_directories(lodi_client PRIVATE common)

add_executable(lodi_server
    lodi-server/lodi_server.c
    ${COMMON_SRC}
    common/pke_messaging.c
    common/pke_messaging.h
    common/tfa_messaging.c
    common/tfa_messaging.h
    common/lodi_messaging.c
    common/lodi_messaging.h
)
target_include_directories(lodi_server PRIVATE common)

add_executable(pke_server
    pke-server/pke_server.c
    ${COMMON_SRC}
)
target_include_directories(pke_server PRIVATE common)

add_executable(tfa_client
    tfa-client/tfa_client.c
    ${COMMON_SRC}
    common/pke_messaging.c
    common/pke_messaging.h
    common/tfa_messaging.c
    common/tfa_messaging.h
)
target_include_directories(tfa_client PRIVATE common)

add_executable(tfa_server
    tfa-server/tfa_server.c
    ${COMMON_SRC}
    common/tfa_messaging.c
    common/tfa_messaging.h
)
target_include_directories(tfa_server PRIVATE common)
