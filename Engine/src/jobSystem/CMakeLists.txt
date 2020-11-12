cmake_minimum_required(VERSION 3.16)
project(JobSystem)

file(GLOB SRC "*.cpp")

add_library(JobSystem ${SRC})
target_link_libraries(JobSystem )