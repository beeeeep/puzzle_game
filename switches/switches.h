#ifndef SWITCHES_H
#define SWITCHES_H

#include <stdio.h>
#include <stdbool.h>
#include "../roll/roll.h"
#include <string.h>
#define NO_OF_SWITCHES_PER_LINE 5

#define NO_OF_3_WAY_LINES 4
#define MAX_GAME_TIME_IN_MS 20000
#define MIN__GAME_TIME_IN_MS 2000
#define NO_OF_LEVELS 20
#define MAX_RANDOM_ATTEMPTS 65536
#define ERROR_REPORT 1;
#define NUMBER_OF_DISABLED_SWITCHES 3
#define TIMESTAMP_TYPE long long


typedef struct rotary_enc
{
    int direction;
    unsigned char button;
}rotary_enc_t;

typedef enum
{
   high_switch = 0,
   mid_switch,
   low_switch
} switch_pos_t;




static int key;

#define POSSIBLE_PATHS (NO_OF_SWITCHES_PER_LINE * NO_OF_3_WAY_LINES)

typedef struct three_way_switch
{
   /* data */
   struct three_way_switch *neighbor_switch[3];
   unsigned char line;
   unsigned char col;
   bool selected;
   bool start_point;
   bool end_point;
   bool has_power;
   switch_pos_t possition;
   struct three_way_switch *connected_to_prev[3];
   char display;
} three_way_switch_t;

typedef struct control_index
{
   int line;
   int column;
} control_index_t;

int switches_init(three_way_switch_t switches[NO_OF_3_WAY_LINES][NO_OF_SWITCHES_PER_LINE], int start_nodes[NO_OF_3_WAY_LINES], int end_nodes[NO_OF_3_WAY_LINES], int end_goal);
int switches_randomize_possition(three_way_switch_t switches[NO_OF_3_WAY_LINES][NO_OF_SWITCHES_PER_LINE], int end_nodes[NO_OF_3_WAY_LINES], int end_goal);
void switches_connect(three_way_switch_t switches[NO_OF_3_WAY_LINES][NO_OF_SWITCHES_PER_LINE], int start_nodes[NO_OF_3_WAY_LINES], int end_nodes[NO_OF_3_WAY_LINES], int end_goal);
void switches_distribute_power(three_way_switch_t switches[NO_OF_3_WAY_LINES][NO_OF_SWITCHES_PER_LINE], int start_nodes[NO_OF_3_WAY_LINES], int end_nodes[NO_OF_3_WAY_LINES]);
int switches_control(three_way_switch_t switches[NO_OF_3_WAY_LINES][NO_OF_SWITCHES_PER_LINE], control_index_t *control, rotary_enc_t *rotary, int *button_pushed_flag);
int switches_verify_possition(three_way_switch_t switches[NO_OF_3_WAY_LINES][NO_OF_SWITCHES_PER_LINE], int line, int col, switch_pos_t pos);
int switches_time_calculate(long long current_time, unsigned int max_time_in_ms, unsigned char time_count_active_flag);
void switches_time_reset(unsigned long current_time);
int switches_time_get_level_time(int current_level);



#endif //SWITCHES_H