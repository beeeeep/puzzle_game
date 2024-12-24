#include "game_state.h"

#include <string.h>

void init_game_state(game_state_t* game_state)
{
   memset(game_state->map.switches, 0, sizeof(game_state->map.switches));
   memset(game_state->map.end_nodes, 0, sizeof(game_state->map.end_nodes));
   memset(game_state->map.start_nodes, 0, sizeof(game_state->map.start_nodes));
   game_state->rotary.direction = 0;
   game_state->rotary.button = 0;
   game_state->current_level = 0;
   reset_control(&game_state->control);
}

void reset_control(control_index_t* control)
{
   control->line = 0;
   control->column = 0;
}