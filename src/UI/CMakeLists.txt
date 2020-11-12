cmake_minimum_required(VERSION 3.16)
project(UI)

file(GLOB SRC "*.cpp")

add_library(UI ${SRC})
target_link_libraries(UI )