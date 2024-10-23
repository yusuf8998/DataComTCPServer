cmake_minimum_required(VERSION 3.12)

project(DataComTCPServer)

add_executable(DataComTCPServer
    "src/GameLogic.c"
    "src/TCPServer.c"
    "src/App.c"
)