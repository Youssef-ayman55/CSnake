cmake_minimum_required(VERSION 3.30)
project(snake C)

set(CMAKE_C_STANDARD 11)
find_package(SDL2 REQUIRED)

add_executable(snake main.c)
include_directories(snake ${SDL2_INCLUDE_DIRS})
target_link_libraries(snake ${SDL2_LIBRARIES})

