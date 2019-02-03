cmake_minimum_required(VERSION 2.8.11)
project(xsonrpc_examples)

set(examples)

include_directories(BEFORE ${XSONRPC_INCLUDE_DIR})

add_executable(testserver testserver.cpp)
target_link_libraries(testserver xsonrpc)
list(APPEND examples testserver)

add_executable(testclient testclient.cpp)
target_link_libraries(testclient xsonrpc)
list(APPEND examples testclient)

add_executable(base64 base64.cpp)
target_link_libraries(base64 xsonrpc)
list(APPEND examples base64)

add_custom_target(examples DEPENDS ${examples})
