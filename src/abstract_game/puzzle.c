#include "puzzle.h"
#include <stdlib.h>
#include "src/switches/game_state.h"

userInterface_t* ui;

bool has_player_won_level(const int end_nodes[], const int end_goal)
{
   return end_nodes[end_goal] == 1;
}

int puzzleGame(void (*init_ui_structures)(userInterface_t**), void (*delete_ui_structures)(userInterface_t**))
{
   game_state_t game_state;
   init_game_state(&game_state);
   int time_left = 0;
   int button_pushed_flag = 0;
   roll_init();

   if (does_fail(switches_init(&game_state)))
   {
      return 1;
   }

   init_ui_structures(&ui);
   (*ui->initVisuals)();
   (*ui->initControls)();
   LOOP_FOREVER_MAX_GUARD(loops_failed, 10)
   {
      (*ui->get_controls_status)(&game_state.rotary);
      switches_distribute_power(&game_state.map);
      switches_control(&game_state, &button_pushed_flag);

      if (game_state.current_level == 0 && button_pushed_flag == 0)
      {
         switches_time_reset(millis_timestamp());
      }
      sentinel_check_block(
         switches_time_calculate(millis_timestamp(), switches_time_get_level_time(game_state.current_level % 20), 1, &time_left), loops_failed);

      (*ui->drawLevel)(&game_state.map);
      (*ui->appendInfo)(game_state.map.line_end_goal, time_left, game_state.current_level);

      const bool win = has_player_won_level(game_state.map.end_nodes, game_state.map.line_end_goal);
      if (win || time_left == 0)
      {
         if (win)
         {
            game_state.current_level++;
            sleep(5);
         }
         else
         {
            button_pushed_flag=0;
            game_state.current_level = 0;
         }
         if (game_state.current_level == NO_OF_LEVELS)
         {
            // player won the complete game
            // for now reset back
            game_state.current_level = 0;
         }
         // get new level
         exit_on_fail(switches_init(&game_state));
         reset_control(&game_state.control);
         switches_time_reset(millis_timestamp());
      }
   }
   (*ui->terminate)();
   delete_ui_structures(&ui);
   return 0;
}
