#include "gui.h"

#ifdef CMAKE_BUILD

#include "switches/switches.h"
#include "misc.h"

#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

#define WIN_X 4
#define WIN_Y 4

#define WIDTH (NO_OF_SWITCHES_PER_LINE * 10) - 1
#define HEIGHT NO_OF_3_WAY_LINES * 4
#define SWITCH_INIT_COL 11
#define SWITCH_INIT_LINE 2
#define SWITCH_COL_DISTANCE 7
#define SWITCH_LINE_DISTANCE 2




WINDOW *game_window;
int startx = 0;
int starty = 0;
int switch_end_points_col[NO_OF_SWITCHES_PER_LINE];
int switch_end_points_line[NO_OF_3_WAY_LINES];

bool windowNeedsRefresh = false;

void initTerminalScreen()
{
  for (int i = 0; i < NO_OF_SWITCHES_PER_LINE; i++)
  {
    switch_end_points_col[i] = SWITCH_INIT_COL + i * SWITCH_COL_DISTANCE;
  }
  for (int i = 0; i < NO_OF_3_WAY_LINES; i++)
  {
    switch_end_points_line[i] = SWITCH_INIT_LINE + i * SWITCH_LINE_DISTANCE;
  }

  fflush(stdout);
  initscr();
  curs_set(0);
  clear();
  noecho();
  startx = (80 - WIDTH) / 2;
  starty = (24 - HEIGHT) / 2;

  game_window = newwin(HEIGHT, WIDTH, starty, startx);
  start_color();
  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(2, COLOR_YELLOW, COLOR_BLACK);
  wbkgd(game_window, COLOR_PAIR(0));
  nodelay(game_window, TRUE);
}

void initKeyboard()
{
  keypad(game_window, TRUE);
}

void translateKeyboardKeyToRotaryEncoder(rotary_enc_t *rotary)
{
  int key_pressed = wgetch(game_window);
  static long long input_timestamp;

  if (millis_timestamp() - input_timestamp < 5)
  {
    flushinp();
  }

  switch (key_pressed)
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
void refreshWindowIfNeeded()
{
  if (windowNeedsRefresh)
  {
    wrefresh(game_window); /* Print it on to the real screen */
    windowNeedsRefresh = false;
  }
  else
  {
   windowNeedsRefresh = true;
  }
}

void set_switches_display_character(three_way_switches_array_t switches)
{
    // Connect the switches
    for (int line = 0; line < NO_OF_3_WAY_LINES; line++)
    {
        for (int col = 0; col < NO_OF_SWITCHES_PER_LINE; col++)
        {
            switch (switches[line][col].position)
            {
            case high_switch:
                switches[line][col].display = '/';
                break;
            case mid_switch: 
                // to avoid having to create two connections per switch, if the switch on the same life has power, 
                // prioritize it over the low and high switches that might be connected
                switches[line][col].display = '=';
                break;
            case low_switch:
                switches[line][col].display = '\\';
                break;
            default:
                switches[line][col].display = 'X';
            }
        }
    }
}


void markActiveSegments(int Z[NO_OF_3_WAY_LINES][NO_OF_SWITCHES_PER_LINE + 1], const three_way_switches_array_t switches, int8_t startx)
{
    Z[startx][0] = 1;
    int newk;
    int k = startx;
    for(int c = 1; c < NO_OF_SWITCHES_PER_LINE + 1; ++c)
    {
       newk = k;
       switch(switches[k][c-1].position)
       {
        case mid_switch:
            {
            Z[k][c] = Z[k][c-1];
            newk = k;
            break;
            }
        case low_switch:
            if (k + 1 < 5) {
                Z[k+1][c] = Z[k][c-1];
                newk = k +1;
            }
            break;
        case high_switch:
            if (k -1 >= 0)
            {
                Z[k-1][c] = Z[k][c-1];
                newk = k -1;
            }
            break;
        default:
           LOG_ERROR("No such switch");
       }
       k = newk;
    }
}

void print(map_t * map)
{
  set_switches_display_character(map->switches);
  const int * start_nodes = map->start_nodes;

  static long flash_on_timestamp;
  static int flash_flag;

  if (millis_timestamp() - flash_on_timestamp > 200)
  {
    flash_on_timestamp = millis_timestamp();
    (flash_flag == 0) ? (flash_flag = 1) : (flash_flag = 0);
  }
 
  wmove(game_window, 1, 0);

  box(game_window, 0, 0);
  wprintw(game_window, "\n");
  int activeSegments[5][6] = {0};
  int start_line = -1;
  for (int i = 0; i < NO_OF_3_WAY_LINES; ++i) { if (start_nodes[i] !=0) {start_line = i;} }
  if (start_line < 0)
  {
    LOG_ERROR(" start_line = %d < 0", start_line);
    return;
  }
  markActiveSegments(activeSegments, map->switches, start_line);
  
  for (int line = 0; line < NO_OF_3_WAY_LINES; line++)
  {
    if (start_nodes[line] == 1)
    {
      wprintw(game_window, " 0###");
    }
    else
    {
      wprintw(game_window, " O---");
    }

    for (int col = 0; col < NO_OF_SWITCHES_PER_LINE; col++)
    {
      wprintw(game_window, "%d", activeSegments[line][col]);
      if (activeSegments[line][col] != 0)
      {
        
        wprintw(game_window, "###");
      }
      else
      {
        wprintw(game_window, "---");
      }

      if (flash_flag && map->switches[line][col].selected == true)
      {
        wprintw(game_window, "* *");
      }
      else
      {
        const int color_index = (map->switches[line][col].switch_color == red) ? 1 : 2;
        wattron(game_window, COLOR_PAIR(color_index));
        wprintw(game_window, "*%c*", map->switches[line][col].display);
        wattroff(game_window, COLOR_PAIR(color_index));
      }
    }

    if (activeSegments[line][5] == 1)
    {
      wprintw(game_window, "###0\n");
    }
    else
    {
      wprintw(game_window, "---O\n");
    }
    wprintw(game_window, "\n");
  }

  for (int line = 0; line < NO_OF_3_WAY_LINES; line++)
  {

    for (int col = 0; col < NO_OF_SWITCHES_PER_LINE; col++)
    {
      if (map->switches[line][col].display == '/')
      {
        if (flash_flag && map->switches[line][col].selected == true)
        {
          mvwprintw(game_window, switch_end_points_line[line] - 1, switch_end_points_col[col], " ");
        }
        else
        {
          const int color_index = (map->switches[line][col].switch_color == red) ? 1 : 2;
          wattron(game_window, COLOR_PAIR(color_index));
          mvwprintw(game_window, switch_end_points_line[line] - 1, switch_end_points_col[col], "/");
          wattroff(game_window, COLOR_PAIR(color_index));
        }
      }

      if (map->switches[line][col].display == '\\')
      {
        if (flash_flag && map->switches[line][col].selected == true)
        {
          mvwprintw(game_window, switch_end_points_line[line] + 1, switch_end_points_col[col], " ");
        }
        else
        {
          const int color_index = (map->switches[line][col].switch_color == red) ? 1 : 2;
          wattron(game_window, COLOR_PAIR(color_index));
          mvwprintw(game_window, switch_end_points_line[line] + 1, switch_end_points_col[col], "\\");
          wattroff(game_window, COLOR_PAIR(color_index));
        }
      }
    }
  }
  refreshWindowIfNeeded();
}

void appendInfo(const int end_goal, const int time_left, const int level_no)
{
  mvwprintw(game_window, 11, 3, "GOAL: %u LVL:%02u TIME:%01u", end_goal + 1, level_no + 1, time_left);
  refreshWindowIfNeeded();
}

void terminateNcursesTerminal()
{
  clrtoeol();
  refresh();
  endwin();
}

void initLevel(unsigned char current_level)
{
  // do nothing
}

void init_gui_structures(userInterface_t** gui)
{
  *gui = (userInterface_t*)malloc(sizeof(userInterface_t));
  (*gui)->initVisuals = initTerminalScreen;
  (*gui)->initControls = initKeyboard;
  (*gui)->init_level = initLevel;
  (*gui)->drawLevel = print;
  (*gui)->appendInfo = appendInfo;
  (*gui)->get_controls_status = translateKeyboardKeyToRotaryEncoder;
  (*gui)->terminate = terminateNcursesTerminal;
}

void delete_gui_structures(userInterface_t** gui)
{
  free(*gui);
  *gui = NULL;
}


#endif // CMAKE_BUILD
