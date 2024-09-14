# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name
project(puzzle)

# Set the C standard
set(CMAKE_C_STANDARD 99)

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

# Include directories
include_directories(switches roll)

# Source files
set(SOURCE_FILES puzzle.c switches/switches.c roll/roll.c)

# Executable target
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Link libraries
target_link_libraries(${PROJECT_NAME} ncurses)
