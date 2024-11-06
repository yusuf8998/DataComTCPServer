cmake_minimum_required(VERSION 3.12)

project(DataComTCPServer)

add_executable(DataComTCPServer
    "src/GameLogic.h"
    "src/TCPServer.h"
    "src/GameLogic.c"
    "src/TCPServer.c"
    "src/App.c"
)

target_compile_definitions(DataComTCPServer PUBLIC "$<$<CONFIG:DEBUG>:DEBUG>")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

if (WIN32)
    target_link_libraries(DataComTCPServer ws2_32)
endif()