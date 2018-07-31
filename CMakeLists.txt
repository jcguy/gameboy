cmake_minimum_required(VERSION 3.11)
project(gameboy C)

set(CMAKE_C_STANDARD 11)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(gameboy main.c registers.h instructions.h typedefs.h)
target_link_libraries(gameboy ${SDL2_LIBRARIES})