#ifndef SWITCHES_H
#define SWITCHES_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "../misc.h"
#include <string.h>

#define NO_OF_SWITCHES_PER_LINE 5

#define NO_OF_3_WAY_LINES 5
#define MAX_GAME_TIME_IN_MS 20000
#define MIN_GAME_TIME_IN_MS 2000
#define MAX_GAME_MOVEMENTS 16
#define MIN_GAME_MOVEMENTS 5

#define NO_OF_LEVELS 20
#define MAX_RANDOM_ATTEMPTS 65536
#define ENABLE_LOGGING 1;
#define NUMBER_OF_DISABLED_SWITCHES 3
#define TIMESTAMP_TYPE long long

#define MAX_NUM_SWITCHES_TO_DISTRIBUTE_POWER 3

typedef struct rotary_enc
{
    int direction;
    unsigned char button;
} rotary_enc_t;

typedef enum
{
   high_switch = 0,
   mid_switch,
   low_switch
} switch_pos_t;

typedef enum switch_color_t
{
   red = 0,
   yellow,
} switch_color_t;

typedef struct control_index_t
{
   int line;
   int column;
} control_index_t;

#define INVALID_CONTROL_INDEX ((control_index_t){.line = -1, .column = -1})

#define POSSIBLE_PATHS (NO_OF_SWITCHES_PER_LINE * NO_OF_3_WAY_LINES)

typedef struct three_way_switch
{
   /* data */
   struct three_way_switch *neighbor_switch[MAX_NUM_SWITCHES_TO_DISTRIBUTE_POWER];
   unsigned char line;
   unsigned char col;
   bool selected;
   bool start_point;
   bool end_point;
   bool has_power;
   switch_pos_t position;
   struct three_way_switch *connected_to_prev[MAX_NUM_SWITCHES_TO_DISTRIBUTE_POWER];
   char display;
   switch_color_t switch_color;
   control_index_t binded_switch_index;
} three_way_switch_t;

typedef three_way_switch_t three_way_switches_array_t[NO_OF_3_WAY_LINES][NO_OF_SWITCHES_PER_LINE];

typedef struct map
{
  three_way_switches_array_t switches;
  int start_nodes[NO_OF_3_WAY_LINES];
  int end_nodes[NO_OF_3_WAY_LINES];
  int line_end_goal;
} map_t;

typedef struct game_state_t
{
  map_t map;
  control_index_t control;
  rotary_enc_t rotary;
  int current_level;
} game_state_t;


function_status_t switches_init(game_state_t * game_state);
function_status_t switches_randomize_possition(three_way_switches_array_t switches, int end_nodes[NO_OF_3_WAY_LINES], int end_goal);
void switches_distribute_power(map_t *map);
function_status_t switches_control(game_state_t* game_state, int *button_pushed_flag);
bool switches_verify_position(three_way_switches_array_t switches, int line, int col, switch_pos_t pos);
function_status_t switches_time_calculate(long long current_time, unsigned int max_time_in_ms, unsigned char time_count_active_flag, int *result);
void switches_time_reset(unsigned long current_time);
int switches_time_get_level_time(int current_level);


#endif //SWITCHES_H