cmake_minimum_required(VERSION 3.18)
project(tetris)

set(CMAKE_CXX_STANDARD 17)
target_compile_options(-pthread)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(tetris main.cpp tetraminoes.cpp tetraminoes.h IO.cpp IO.h client.cpp CloseServer.cpp)
