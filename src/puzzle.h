#ifndef PUZZLE_H
#define PUZZLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "switches/switches.h"
#include "gui/gui.h"
#include "misc.h"
#include "log.h"
#include "roll/roll.h"

typedef struct userInterface
{
  void (*initVisuals)();
  void (*initControls)();
  void (*drawLevel)(three_way_switches_array_t switches, int start_nodes[NO_OF_3_WAY_LINES], int end_nodes[NO_OF_3_WAY_LINES]);
  void (*appendInfo)(const int end_goal, const int time_left, const int level_no);
  void (*get_controls_status)(rotary_enc_t *rotary);
  void (*terminate)();
} userInterface_t;

typedef struct game_state_t
{
 int a;
} game_state_t;

#ifdef __cplusplus
}
#endif

#endif // PUZZLE_H