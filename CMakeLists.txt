# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name
project(puzzle)

add_definitions(-DCMAKE_BUILD)
# Set the C standard
set(CMAKE_C_STANDARD 23)

# Compiler flags
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

# Include directories
add_subdirectory(src)