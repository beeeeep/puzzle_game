#ifndef PUZZLE_H
#define PUZZLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "src/switches/switches.h"
#include "src/misc.h"
#include "src/log.h"
#include "src/roll/roll.h"


typedef struct userInterface
{
  void (*initVisuals)();
  void (*initControls)();
  unsigned int (*RandomNumberSeed)();
  void (*drawLevel)(map_t* map);
  void (*appendInfo)(const int end_goal, const int time_left, unsigned char numberOfMovements, const int level_no);
  void (*init_level)(unsigned char current_level);
  void (*get_controls_status)(rotary_enc_t *rotary);
  void (*level_win)();
  void (*terminate)();
} userInterface_t;

int puzzleGame(void (*init_ui_structures)(userInterface_t**), void (*delete_ui_structures)(userInterface_t**));
function_status_t puzzleGameInit(game_state_t* game_state, void (*init_ui_structures)(userInterface_t**));
function_status_t puzzleGameMainIteration(game_state_t* game_state);
function_status_t puzzleGameTermination(void (*delete_ui_structures)(userInterface_t**));


#ifdef __cplusplus
}
#endif

#endif // PUZZLE_H