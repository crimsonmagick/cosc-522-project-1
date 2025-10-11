cmake_minimum_required(VERSION 3.15)
project(cosc522_lodi C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

file(GLOB COMMON_SRC CONFIGURE_DEPENDS common/*.c)

add_executable(echo_client
    echo-example/UDPEchoClient.c
    ${COMMON_SRC}
)
target_include_directories(echo_client PRIVATE common)

add_executable(echo_server
    echo-example/UDPEchoServer.c
    ${COMMON_SRC}
)
target_include_directories(echo_server PRIVATE common)
