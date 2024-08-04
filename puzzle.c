
#include <stdio.h>

#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#include <ncurses.h>
#include "switches.h"

WINDOW *game_win;

#define WIN_X 4
#define WIN_Y 4

#define WIDTH 39
#define HEIGHT 12

#define SWITCH_INIT_COL 11
#define SWITCH_INIT_LINE 2
#define SWITCH_COL_DISTANCE 7
#define SWITCH_LINE_DISTANCE 2

int startx = 0;
int starty = 0;

int switch_end_points_col[NO_OF_SWITCHES_PER_LINE];
int switch_end_points_line[NO_OF_3_WAY_LINES];


void print(three_way_switch_t switches[NO_OF_3_WAY_LINES][NO_OF_SWITCHES_PER_LINE], int start_nodes[NO_OF_3_WAY_LINES], int end_nodes[NO_OF_3_WAY_LINES], int end_goal, int time_left, int level_no);
long long millis_timestamp(void);
void get_controls_status(int input, rotary_enc_t *rotary);

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

   // init the switch possitions for ncurses

   for (int i = 0; i < NO_OF_SWITCHES_PER_LINE; i++)
   {
      switch_end_points_col[i] = SWITCH_INIT_COL + i * SWITCH_COL_DISTANCE;
   }
   for (int i = 0; i < NO_OF_3_WAY_LINES; i++)
   {
      switch_end_points_line[i] = SWITCH_INIT_LINE + i * SWITCH_LINE_DISTANCE;
   }

   if (switches_init(switches, start_nodes, end_nodes, end_goal) == 0)
   {
      return 1;
   }

   fflush(stdout);
   initscr();
   curs_set(0);
   clear();
   noecho();
   startx = (80 - WIDTH) / 2;
   starty = (24 - HEIGHT) / 2;

   game_win = newwin(HEIGHT, WIDTH, starty, startx);
   nodelay(game_win, TRUE);
   keypad(game_win, TRUE);

   while (1)
   {
      get_controls_status(wgetch(game_win), &rotary);
      switches_connect(switches, start_nodes, end_nodes, end_goal);
      switches_distribute_power(switches, start_nodes, end_nodes);
      switches_control(switches, &control, &rotary,&button_pushed_flag);

      if (current_level == 0 && button_pushed_flag == 0)
      {
         switches_time_reset(millis_timestamp());
      }
      time_left = switches_time_calculate(millis_timestamp(), switches_time_get_level_time(current_level), 1);
      
      print(switches, start_nodes, end_nodes, end_goal, time_left, current_level);

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
   clrtoeol();
   refresh();
   endwin();
}



void print(three_way_switch_t switches[NO_OF_3_WAY_LINES][NO_OF_SWITCHES_PER_LINE], int start_nodes[NO_OF_3_WAY_LINES], int end_nodes[NO_OF_3_WAY_LINES], int end_goal, int time_left, int level_no)
{

   static long flash_on_timestamp;
   static long flash_off_timestamp;
   static int flash_flag;

   if (millis_timestamp() - flash_on_timestamp > 200)
   {
      flash_on_timestamp = millis_timestamp();
      (flash_flag == 0) ? (flash_flag = 1) : (flash_flag = 0);
   }
   // clear();
   // clrtoeol();

   // printw("\033[H");
   wmove(game_win, 1, 0);

   box(game_win, 0, 0);
   wprintw(game_win, "\n");
   for (int line = 0; line < NO_OF_3_WAY_LINES; line++)
   {
      if (start_nodes[line] == 1)
      {
         wprintw(game_win, " 0###");
      }
      else
      {
         wprintw(game_win, " O---");
      }
      for (int col = 0; col < NO_OF_SWITCHES_PER_LINE; col++)
      {
         if (switches[line][col].has_power == 1)
         {
            wprintw(game_win, "####");
         }
         else
         {
            wprintw(game_win, "----");
         }

         if (flash_flag && switches[line][col].selected == true)
         {
            wprintw(game_win, "* *");
         }
         else
         {
            wprintw(game_win, "*%c*", switches[line][col].display);
         }
      }

      if (end_nodes[line] == 1)
      {
         wprintw(game_win, "###0\n");
      }
      else
      {
         wprintw(game_win, "---O\n");
      }
      wprintw(game_win, "\n");
   }

   for (int line = 0; line < NO_OF_3_WAY_LINES; line++)
   {

      for (int col = 0; col < NO_OF_SWITCHES_PER_LINE; col++)
      {
         if (switches[line][col].display == '/')
         {
            if (flash_flag && switches[line][col].selected == true)
            {
               mvwprintw(game_win, switch_end_points_line[line] - 1, switch_end_points_col[col], " ");
            }
            else
            {
               mvwprintw(game_win, switch_end_points_line[line] - 1, switch_end_points_col[col], "/");
            }
         }

         if (switches[line][col].display == '\\')
         {
            if (flash_flag && switches[line][col].selected == true)
            {
               mvwprintw(game_win, switch_end_points_line[line] + 1, switch_end_points_col[col], " ");
            }
            else
            {
               mvwprintw(game_win, switch_end_points_line[line] + 1, switch_end_points_col[col], "\\");
            }
         }
      }
   }
   mvwprintw(game_win, 10, 5, "GOAL: %u LVL:%02u TIME:%01u", end_goal + 1, level_no + 1, time_left);
   // mvwprintw(game_win, 9, 10, "%u", *col_index);
   wrefresh(game_win); /* Print it on to the real screen */
}

// double possible_paths=pow(2,(NO_OF_2_WAY_LINES*NO_OF_SWITCHES_PER_LINE))*pow(3,(NO_OF_3_WAY_LINES*NO_OF_SWITCHES_PER_LINE));//+pow(NO_OF_2_WAY_LINES*NO_OF_SWITCHES_PER_LINE,2);

// printw("Possible paths:%f\n",possible_paths);

long long millis_timestamp()
{
   struct timeval te;
   gettimeofday(&te, NULL);                                         // get current time
   long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000; // calculate milliseconds
   // printw("milliseconds: %lld\n", milliseconds);
   return milliseconds;
}

void get_controls_status(int input, rotary_enc_t *rotary)
{
   int key_pressed = -1;
   static long long input_timestamp;

   if (millis_timestamp() - input_timestamp < 5)
   {
      flushinp();
   }

   key_pressed = input;

   switch (input)
   {
   case KEY_RIGHT:
      rotary->direction = 1;
      input_timestamp = millis_timestamp();
      break;
   case KEY_LEFT:
      rotary->direction = -1;
      input_timestamp = millis_timestamp();
      break;
   case KEY_UP:
      rotary->button = 1;
      input_timestamp = millis_timestamp();
      break;
   default:
      rotary->direction = 0;
      rotary->button = 0;
   }
}
