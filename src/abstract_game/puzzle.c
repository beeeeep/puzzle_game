#include "puzzle.h"
#include <stdlib.h>
#include "src/switches/game_state.h"
#include <time.h>

static userInterface_t* ui;
static int button_pushed_flag;

bool has_player_won_level(const int end_nodes[], const int end_goal);


int puzzleGame(void (*init_ui_structures)(userInterface_t**), void (*delete_ui_structures)(userInterface_t**))
{
   game_state_t game_state;
   exit_on_fail(puzzleGameInit(&game_state, init_ui_structures));
   LOOP_FOREVER_MAX_GUARD(loops_failed, 10)
   {
      sentinel_check_block(puzzleGameMainIteration(&game_state), loops_failed);
      #ifdef __linux__ || __WIN32
      usleep(100 * 1000);
      #endif
   }
   puzzleGameTermination(delete_ui_structures);
   return 0;
}


function_status_t puzzleGameInit(game_state_t* game_state, void (*init_ui_structures)(userInterface_t**))
{
   init_game_state(game_state);
   button_pushed_flag = 0;
   roll_init();

   init_ui_structures(&ui);
   (*ui->initVisuals)();
   (*ui->initControls)();
   (*ui->init_level)(0);
   if (does_fail(switches_init(game_state)))
   {
      return FAILURE;
   }
   
   LOG_INFO("Abstract game initialized");

  
   return SUCCESS;
}

function_status_t puzzleGameMainIteration(game_state_t* game_state)
{
   static long long timestamp_ms;
   (*ui->get_controls_status)(&game_state->rotary);
   switches_distribute_power(&game_state->map);
   switches_control(game_state, &button_pushed_flag);

   if (game_state->current_level == 0 && button_pushed_flag == 0)
   {
      switches_time_reset(millis_timestamp());     
   }
   return_on_fail(
      switches_time_calculate(millis_timestamp(), switches_time_get_level_time(game_state->current_level % 20), 1, &game_state->time_left));
  
   (*ui->drawLevel)(&game_state->map);
   (*ui->appendInfo)(game_state->map.line_end_goal, game_state->time_left, game_state->current_level);
   const bool win = has_player_won_level(game_state->map.end_nodes, game_state->map.line_end_goal);
   
   if (win || game_state->time_left <= 0)
   {
      if (win)
      {
         LOG_INFO("Game win %d,%d,%d,%d,%d", (int)game_state->map.end_nodes[0], (int)game_state->map.end_nodes[1], (int)game_state->map.end_nodes[2], (int)game_state->map.end_nodes[3], (int)game_state->map.end_nodes[4]); 
         game_state->current_level++;
         (*ui->drawLevel)(&game_state->map);
         (*ui->appendInfo)(game_state->map.line_end_goal, game_state->time_left, game_state->current_level);
         timestamp_ms=millis_timestamp();
         while(millis_timestamp()-timestamp_ms<3000);
      }
      else
      {
         LOG_INFO("Out of time");
         button_pushed_flag=0;
         game_state->current_level = 0;
      }
      if (game_state->current_level == NO_OF_LEVELS)
      {
         // player won the complete game
         // for now reset back
         game_state->current_level = 0;
         LOG_INFO("Player won all %d levels", NO_OF_LEVELS);
      }
      // get new level
      exit_on_fail(switches_init(game_state));
      reset_control(&game_state->control);    
      (*ui->init_level)(game_state->current_level);
      timestamp_ms=millis_timestamp();
      while(millis_timestamp()-timestamp_ms<1000);
      switches_time_reset(millis_timestamp());
      
   }
   return SUCCESS;
}

function_status_t puzzleGameTermination(void (*delete_ui_structures)(userInterface_t**))
{
   (*ui->terminate)();
   delete_ui_structures(&ui);
   return SUCCESS;
}

bool has_player_won_level(const int end_nodes[], const int end_goal)
{
   return end_nodes[end_goal] == 1;
}


void debugPrint()
{
   // FILE* file = fopen("win.txt", "w");
   // fprintf(file, "win %d\n", game_state->map.line_end_goal);
   // for(int i = 0; i < 5; i++)
   // {
   //    fprintf(file, "%d===", game_state->map.start_nodes[i]);
   //    for(int j =0; j < 5; j++)
   //    {
   //       fprintf(file, "%d,", game_state->map.switches[i][j].position);
   //    }
   //    fprintf(file, "--->%d\n", game_state->map.end_nodes[i]);
   // }
   // fclose(file);


}