# Include directories
include_directories(switches roll interfaces servo_controller gui .)


# Source files
set(UI_HEADER_FILES gui/gui.h)
set(UI_SOURCE_FILES gui/gui.c)
set(CONFIG_HEADER_FILES switches/red_switches.h)
set(HEADER_FILES abstract_game/puzzle.h switches/switches.h switches/game_state.h roll/roll.h interfaces/servo_switches.h servo_controller/servo_controller.h ${UI_HEADER_FILES} misc.h log.h ${CONFIG_HEADER_FILES})
set(SOURCE_FILES abstract_game/puzzle.c switches/switches.c switches/game_state.c roll/roll.c interfaces/servo_switches.c servo_controller/servo_controller.c ${UI_SOURCE_FILES} misc.c)

# Executable target
set(TARGET_FILE puzzle_game_pc.c)
add_executable(${PROJECT_NAME} ${TARGET_FILE} ${SOURCE_FILES} ${HEADER_FILES})

# Link libraries
target_link_libraries(${PROJECT_NAME} ncurses)