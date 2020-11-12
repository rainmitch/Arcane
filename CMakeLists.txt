cmake_minimum_required(VERSION 3.16)
project(Arcane)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "--std=c++17 -O2 -g3 -mconsole -fopenmp -pthread")

file(GLOB SRC "src/*.cpp")

include_directories(src/headers)

add_subdirectory(src/main)
add_subdirectory(src/jobSystem)
add_subdirectory(src/windowRendering)

add_executable(Arcane ${SRC} src/jobSystem/semaphore.cpp src/headers/arcane/jobSystem/semaphore.h)
target_link_libraries(Arcane Main JobSystem WindowRendering)