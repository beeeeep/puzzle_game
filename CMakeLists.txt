# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name
project(puzzle)

# Set the C standard
set(CMAKE_C_STANDARD 23)

# Compiler flags
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

# Include directories
include_directories(switches roll interfaces servo_controller gui .)

# Source files
set(CONFIG_HEADER_FILES switches/red_switches.h)
set(HEADER_FILES puzzle.h switches/switches.h roll/roll.h interfaces/servo_switches.h servo_controller/servo_controller.h gui/gui.h misc.h ${CONFIG_HEADER_FILES})
set(SOURCE_FILES puzzle.c switches/switches.c roll/roll.c interfaces/servo_switches.c servo_controller/servo_controller.c gui/gui.c misc.c)

# Executable target
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})

# Link libraries
target_link_libraries(${PROJECT_NAME} ncurses)
