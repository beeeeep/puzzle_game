#ifndef PUZZLE_H
#define PUZZLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "switches/switches.h"
#include "misc.h"
#include "log.h"
#include "roll/roll.h"


typedef struct userInterface
{
  void (*initVisuals)();
  void (*initControls)();
  void (*drawLevel)(map_t* map);
  void (*appendInfo)(const int end_goal, const int time_left, const int level_no);
  void (*get_controls_status)(rotary_enc_t *rotary);
  void (*terminate)();
} userInterface_t;

int puzzleGame(void (*init_ui_structures)(userInterface_t**), void (*delete_ui_structures)(userInterface_t**));

#ifdef __cplusplus
}
#endif

#endif // PUZZLE_H