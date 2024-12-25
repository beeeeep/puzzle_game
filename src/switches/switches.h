#ifndef SWITCHES_H
#define SWITCHES_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "../misc.h"
#include <string.h>
#include "game_state.h"

function_status_t switches_init(game_state_t * game_state);
function_status_t switches_randomize_possition(three_way_switches_array_t switches, int end_nodes[NO_OF_3_WAY_LINES], int end_goal);
void switches_distribute_power(map_t *map);
function_status_t switches_control(game_state_t* game_state, int *button_pushed_flag);
bool switches_verify_position(three_way_switches_array_t switches, int line, int col, switch_pos_t pos);
function_status_t switches_time_calculate(long long current_time, unsigned int max_time_in_ms, unsigned char time_count_active_flag, int *result);
void switches_time_reset(unsigned long current_time);
int switches_time_get_level_time(int current_level);


#endif //SWITCHES_H