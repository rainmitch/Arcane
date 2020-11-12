cmake_minimum_required(VERSION 3.16)
project(WindowRendering)

#list(APPEND CMAKE_MODULE_PATH ${CMAKE_HOME_DIRECTORY}/cmake/sdl2)
list(APPEND CMAKE_PREFIX_PATH $ENV{PATH})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

file(GLOB SRC *.cpp OpenGL43/*.cpp SDL2/*.cpp)
add_library(WindowRendering ${SRC})
target_link_libraries(WindowRendering SDL2::SDL2-static GLEW::glew_s)
