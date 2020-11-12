cmake_minimum_required(VERSION 3.16)
project(Main)

file(GLOB SRC "*.cpp")

add_library(Main ${SRC})
target_link_libraries(Main WindowRendering)