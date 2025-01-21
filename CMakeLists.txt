cmake_minimum_required(VERSION 3.30)
project(snake C)

set(CMAKE_C_STANDARD 11)
find_package(SDL2 REQUIRED)

FIND_PACKAGE(PkgConfig REQUIRED)

PKG_CHECK_MODULES(GLIB REQUIRED glib-2.0)

INCLUDE_DIRECTORIES(
        src/include/
        ${GLIB_INCLUDE_DIRS}
)

LINK_DIRECTORIES(${GLIB_LIBRARY_DIRS})

FILE(GLOB SRC src/*.c)

add_executable(snake main.c)
include_directories(snake ${SDL2_INCLUDE_DIRS})
target_link_libraries(snake ${SDL2_LIBRARIES})
TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} ${GLIB_LIBRARIES})
