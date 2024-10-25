#include "puzzle.h"
#include "gui/gui.h"
#include <stdlib.h>

userInterface_t* gui;


bool has_player_won_level(const int end_nodes[], const int end_goal)
{
   return end_nodes[end_goal] == 1;
}

void reset_control(control_index_t* control)
{
   control->line = 0;
   control->column = 0;
}

init_game_state(game_state_t* game_state)
{
   memset(game_state->map.switches, 0, sizeof(game_state->map.switches));
   memset(game_state->map.end_nodes, 0, sizeof(game_state->map.end_nodes));
   memset(game_state->map.start_nodes, 0, sizeof(game_state->map.start_nodes));
   game_state->rotary.direction = 0;
   game_state->rotary.button = 0;
   game_state->current_level = 0;
   reset_control(&game_state->control);
}

game_state_t game_state;

int main(int argc, char **argv)
{
   init_game_state(&game_state);
   int movements_left = 4;
   int time_left = 0;
   int end_goal;
   int current_level = 0;
   int button_pushed_flag = 0;
   roll_init();

   if (does_fail(switches_init(&game_state)))
   {
      return 1;
   }

   init_gui_structures(&gui);
   (*gui->initVisuals)();
   (*gui->initControls)();

   LOOP_FOREVER_MAX_GUARD(loops_failed, 10)
   {
      (*gui->get_controls_status)(&game_state.rotary);
      switches_distribute_power(&game_state.map);
      switches_control(&game_state, &button_pushed_flag);

      if (current_level == 0 && button_pushed_flag == 0)
      {
         switches_time_reset(millis_timestamp());
      }
      sentinel_check_block(
         switches_time_calculate(millis_timestamp(), switches_time_get_level_time(current_level), 1, &time_left), loops_failed);
      end_goal = switches_get_end_goal();

      (*gui->drawLevel)(&game_state.map);
      (*gui->appendInfo)(end_goal, time_left, current_level);

      const bool win = has_player_won_level(game_state.map.end_nodes, end_goal);
      if (win || time_left == 0)
      {
         if (win)
         {
            current_level++;
         }
         else
         {
            button_pushed_flag=0;
            current_level = 0;
         }
         // get new level
         exit_on_fail(switches_init(&game_state));
         reset_control(&game_state.control);
         switches_time_reset(millis_timestamp());
      }
   }
   (*gui->terminate)();
   delete_gui_structures(&gui);
   return 0;
}
