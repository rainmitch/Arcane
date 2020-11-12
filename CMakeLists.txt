cmake_minimum_required(VERSION 3.16)
project(GlobalProject CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} " --std=c++17 -O2 -g3 -mconsole -fopenmp -pthread ")

add_subdirectory(Editor Editor)
add_subdirectory(Engine Engine)