#include "puzzle.h"
#include "gui.h"
#include "misc.h"
#include "log.h"

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

int main(int argc, char **argv)
{
   static three_way_switch_t switches[NO_OF_3_WAY_LINES][NO_OF_SWITCHES_PER_LINE] = {0};

   static int start_nodes[NO_OF_3_WAY_LINES] = {0};
   static int end_nodes[NO_OF_3_WAY_LINES] = {0};
   control_index_t control = {0};
   rotary_enc_t rotary;
   int movements_left = 4;
   int time_left = 0;
   int end_goal = roll(0, NO_OF_SWITCHES_PER_LINE - 1);
   int current_level = 0;
   int button_pushed_flag = 0;

   if (does_fail(switches_init(switches, start_nodes, end_nodes, end_goal)))
   {
      return 1;
   }

   init_gui_structures(&gui);
   (*gui->initVisuals)();
   (*gui->initControls)();

   LOOP_FOREVER_MAX_GUARD(loops_failed, 10)
   {
      (*gui->get_controls_status)(&rotary);
      switches_connect(switches, start_nodes, end_nodes);
      switches_distribute_power(switches, start_nodes, end_nodes);
      switches_control(switches, &control, &rotary,&button_pushed_flag);

      if (current_level == 0 && button_pushed_flag == 0)
      {
         switches_time_reset(millis_timestamp());
      }
      sentinel_check_block(
         switches_time_calculate(millis_timestamp(), switches_time_get_level_time(current_level), 1, &time_left), loops_failed);
      end_goal = switches_get_end_goal();

      (*gui->drawLevel)(switches, start_nodes, end_nodes);
      (*gui->appendInfo)(end_goal, time_left, current_level);

      const bool win = has_player_won_level(end_nodes, end_goal);
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
         exit_on_fail(switches_init(switches, start_nodes, end_nodes, current_level));
         reset_control(&control);
         switches_time_reset(millis_timestamp());
      }
   }
   (*gui->terminate)();
   delete_gui_structures(&gui);
   return 0;
}
