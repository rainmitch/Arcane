cmake_minimum_required(VERSION 3.16)
project(Arcane)

set(CMAKE_CXX_STANDARD 14)

file(GLOB SRC "src/*.cpp")

include_directories(src/headers)


set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${PROJECT_BINARY_DIR}/../)
add_subdirectory(src/main)
add_subdirectory(src/jobSystem)
add_subdirectory(src/windowRendering)

set (ENGINE_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/headers
        CACHE INTERNAL "Engine: Include Directories" FORCE)

add_library (Arcane STATIC ${SRC})
target_link_libraries(Arcane Main JobSystem WindowRendering)
