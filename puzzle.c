#include "puzzle.h"
#include "gui.h"
#include "misc.h"

#include <stdlib.h>


userInterface_t* gui;

int main(int argc, char **argv)
{
   
   // switches[col][line]
   static three_way_switch_t switches[NO_OF_3_WAY_LINES][NO_OF_SWITCHES_PER_LINE] = {0};

   static int start_nodes[NO_OF_3_WAY_LINES] = {0};
   static int end_nodes[NO_OF_3_WAY_LINES] = {0};
   control_index_t control = {0};
   rotary_enc_t rotary;
   int time_left = 0;
   int level_no = 1;
   int end_goal = roll(0, NO_OF_SWITCHES_PER_LINE - 1);
   int current_level = 0;
   int button_pushed_flag = 0;

   if (switches_init(switches, start_nodes, end_nodes, end_goal) == 0)
   {
      return 1;
   }

   init_gui_structures(&gui);
   (*gui->initVisuals)();
   (*gui->initControls)();

   while (1)
   {
      (*gui->get_controls_status)(&rotary);
      switches_connect(switches, start_nodes, end_nodes, end_goal);
      switches_distribute_power(switches, start_nodes, end_nodes);
      switches_control(switches, &control, &rotary,&button_pushed_flag);

      if (current_level == 0 && button_pushed_flag == 0)
      {
         switches_time_reset(millis_timestamp());
      }
      time_left = switches_time_calculate(millis_timestamp(), switches_time_get_level_time(current_level), 1);

      (*gui->drawLevel)(switches, start_nodes, end_nodes, end_goal, time_left, level_no);

      if (end_nodes[end_goal] == 1 || time_left == 0)
      {
         if (end_nodes[end_goal] == 1)
         {
            current_level++;
         }
         else
         {
            button_pushed_flag=0;
            current_level = 0;
         }
         end_goal = roll(0, NO_OF_3_WAY_LINES - 1);
         if (switches_init(switches, start_nodes, end_nodes, end_goal) == 0)
         {
            return 0;
         }
         control.line = 0;
         control.column = 0;

         switches_time_reset(millis_timestamp());
      }
   }
   (*gui->terminate)();
   delete_gui_structures(&gui);
}
