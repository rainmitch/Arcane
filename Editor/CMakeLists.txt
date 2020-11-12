cmake_minimum_required(VERSION 3.16)
project(ArcaneEditor)

set(CMAKE_CXX_STANDARD 14)

file(GLOB SRC "src/*.cpp")

set( EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../)
message(${PROJECT_BINARY_DIR}/../)
include_directories(src/headers)
include_directories (${ENGINE_INCLUDE_DIRS})

add_executable (ArcaneEditor ${SRC})
target_link_libraries(ArcaneEditor Arcane)