#include "gui.h"
#include "switches.h"
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

WINDOW *game_win;
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

  game_win = newwin(HEIGHT, WIDTH, starty, startx);
  start_color();
  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(2, COLOR_YELLOW, COLOR_BLACK);
  wbkgd(game_win, COLOR_PAIR(0));
  nodelay(game_win, TRUE);
}

void initKeyboard()
{
  keypad(game_win, TRUE);
}

void translateKeyboardKeyToRotaryEncoder(rotary_enc_t *rotary)
{
  int key_pressed = wgetch(game_win);
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
    wrefresh(game_win); /* Print it on to the real screen */
    windowNeedsRefresh = false;
  }
  else
  {
   windowNeedsRefresh = true;
  }
}

void print(three_way_switches_array_t switches, int start_nodes[NO_OF_3_WAY_LINES], int end_nodes[NO_OF_3_WAY_LINES])
{

  static long flash_on_timestamp;
  static int flash_flag;

  if (millis_timestamp() - flash_on_timestamp > 200)
  {
    flash_on_timestamp = millis_timestamp();
    (flash_flag == 0) ? (flash_flag = 1) : (flash_flag = 0);
  }
 
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
        const int color_index = (switches[line][col].switch_color == red) ? 1 : 2;
        wattron(game_win, COLOR_PAIR(color_index));
        wprintw(game_win, "####");
        wattroff(game_win, COLOR_PAIR(color_index));
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
  refreshWindowIfNeeded();
}

void appendInfo(const int end_goal, const int time_left, const int level_no)
{
  mvwprintw(game_win, 11, 3, "GOAL: %u LVL:%02u TIME:%01u", end_goal + 1, level_no + 1, time_left);
  refreshWindowIfNeeded();
}

void terminateNcursesTerminal()
{
  clrtoeol();
  refresh();
  endwin();
}

void init_gui_structures(userInterface_t** gui)
{
  *gui = (userInterface_t*)malloc(sizeof(userInterface_t));
  (*gui)->initVisuals = initTerminalScreen;
  (*gui)->initControls = initKeyboard;
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