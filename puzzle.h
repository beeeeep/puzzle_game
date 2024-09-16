#ifndef PUZZLE_H
#define PUZZLE_H

#include "switches/switches.h"

typedef struct userInterface_t{
  void (*initVisuals)();
  void (*initControls)();
  void (*drawLevel)(three_way_switches_array_t switches, int start_nodes[NO_OF_3_WAY_LINES], int end_nodes[NO_OF_3_WAY_LINES], int end_goal, int time_left, int level_no);
  void (*get_controls_status)(rotary_enc_t *rotary);
  void (*terminate)();
} userInterface_t;

#endif // PUZZLE_H