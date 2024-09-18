#include "switches.h"
#include "red_switches.h"

#include "log.h"

static long long switches_timestamp;

static int end_goal = 0;

int minimum(int a, int b)
{
    return (a < b) ? a : b;
}

void resetEndNodes(int end_nodes[])
{
    memset(end_nodes, 0, sizeof(int) * NO_OF_3_WAY_LINES);
}

void default_init_switch(three_way_switches_array_t switches, const int line, const int col);
void default_init_switces(three_way_switches_array_t switches, const int num_lines, const int num_cols);
void init_red_switches(three_way_switches_array_t switches);
void generateAndInitializeStartAndGoal(int start_nodes[], int end_nodes[], int *star_node_line_index, int* end_goal_index, const int current_level);


function_status_t switches_init(three_way_switches_array_t switches, int start_nodes[NO_OF_3_WAY_LINES], int end_nodes[NO_OF_3_WAY_LINES], const int current_level)
{
    unsigned long path_tracing_attemtps = 0;
    int path_found_counter = 0;
    int random_number_of_paths_found = roll(0, 99);
    int start_node_index;
    int end_goal_index;
    generateAndInitializeStartAndGoal(start_nodes, end_nodes, &start_node_index, &end_goal_index, current_level);

    // Find a random path for the end goal
    do
    {
        path_tracing_attemtps++;
        // Initialize the switches
        default_init_switces(switches, NO_OF_3_WAY_LINES, NO_OF_SWITCHES_PER_LINE);
        switches_connect(switches, start_nodes, end_nodes);
        init_red_switches(switches);
        switches_distribute_power(switches, start_nodes, end_nodes);

        if (end_nodes[end_goal_index] == 1)
        {
            path_found_counter++;
            path_tracing_attemtps = 0;
        }
        else if (path_tracing_attemtps >= MAX_RANDOM_ATTEMPTS) // if this is true, it means that the path tracing is stuck
        {
            LOG_ERROR("path tracing stuck");
            return FAILURE; // return error
        }
    } while (path_found_counter < random_number_of_paths_found);

    // Randomize the active switches
    while (end_nodes[end_goal_index] == 1)
    {
        if (does_fail(switches_randomize_possition(switches, end_nodes, end_goal_index)))
        {
            LOG_ERROR("switch randomizing stuck");
            return FAILURE; // return error
        }
        // Run this again to redistribute power after randomizing
        switches_connect(switches, start_nodes, end_nodes);
        switches_distribute_power(switches, start_nodes, end_nodes);
    }

    return SUCCESS;
}

void init_nodes(int start_nodes[], int end_nodes[], const int star_node_line_index)
{
    memset(start_nodes, 0, sizeof(int) * NO_OF_3_WAY_LINES);
    memset(end_nodes, 0, sizeof(int) * NO_OF_3_WAY_LINES);
    start_nodes[star_node_line_index] = 1;
}

void generateAndInitializeStartAndGoal(int start_nodes[], int end_nodes[], int *star_node_line_index, int* end_goal_index, const int current_level)
{
    *star_node_line_index = roll(0, NO_OF_3_WAY_LINES - 1);
    *end_goal_index = roll(0, NO_OF_3_WAY_LINES - 1);
    end_goal = *end_goal_index;
    // Reset all the nodes and switches
    init_nodes(start_nodes, end_nodes, *star_node_line_index);
}

void default_init_switces(three_way_switches_array_t switches, const int num_lines, const int num_cols)
{
    memset(switches, 0, sizeof(three_way_switch_t) * NO_OF_3_WAY_LINES * NO_OF_SWITCHES_PER_LINE);
    for (int line = 0; line < num_lines; line++)
    {
        for (int col = 0; col < num_cols; col++)
        {
            // Initialize the switch
            default_init_switch(switches, line, col);                
        }
    }
}

void default_init_switch(three_way_switches_array_t switches, const int line, const int col)
{
    three_way_switch_t* sw_init = &(switches[line][col]);
    sw_init->col = col;
    sw_init->line = line;
    sw_init->neighbor_switch[high_switch] = NULL;
    sw_init->neighbor_switch[mid_switch] = NULL;
    sw_init->neighbor_switch[low_switch] = NULL;
    sw_init->switch_color = yellow;
    sw_init->binded_switch_index = INVALID_CONTROL_INDEX;

    // Connect to top switch
    if (line > 0 && col < NO_OF_SWITCHES_PER_LINE)
    {
        sw_init->neighbor_switch[high_switch] = &switches[line - 1][col];
    }
    // connect to mid switch void switches_control(three_way_switch_t switches[NO_OF_3_WAY_LINES][NO_OF_SWITCHES_PER_LINE], char **argv)
    if (col < NO_OF_SWITCHES_PER_LINE - 1)
    {
        sw_init->neighbor_switch[mid_switch] = &switches[line][col + 1];
    }
    // connect to low switch
    if (line < (NO_OF_3_WAY_LINES - 1) && col < NO_OF_SWITCHES_PER_LINE)
    {
        sw_init->neighbor_switch[low_switch] = &switches[line + 1][col];
    }

    // Top line check
    sw_init->position = (switch_pos_t)roll(high_switch, low_switch);

    if (switches[line - 1][col].position == low_switch && line > 0)
    {
        sw_init->position = (switch_pos_t)roll(mid_switch, low_switch);
    }
}

void init_red_switches(three_way_switches_array_t switches)
{
    for (int i = 0; i < NUM_RED_SWITCHES; i++)
    {
        const int line = red_switches_indices[i].line;
        const int col = red_switches_indices[i].column;
        switches[line][col].switch_color = red;
        switches[line][col].binded_switch_index.line = red_switches_connections[i].line_b;
        switches[line][col].binded_switch_index.column = red_switches_connections[i].col_b;
    }
}

function_status_t switches_randomize_possition(three_way_switch_t switches[NO_OF_3_WAY_LINES][NO_OF_SWITCHES_PER_LINE], int end_nodes[NO_OF_3_WAY_LINES], int end_goal)
{
    // switch 3 of the critical switches to a random setting
    int previous_changed_sw_index[3] = {0};
   
    int active_sw_line[NO_OF_3_WAY_LINES] = {0};
    int active_sw_col[NO_OF_SWITCHES_PER_LINE] = {0};
     
    int number_of_active_switches = 0;
    int number_of_changes = 0;
    int iteration_counter = 0;

    for (int line = 0; line < NO_OF_3_WAY_LINES; line++)
    {
        for (int col = 0; col < NO_OF_SWITCHES_PER_LINE; col++)
        {
            if (switches[line][col].has_power == 1)
            {
                active_sw_line[number_of_active_switches] = line;
                active_sw_col[number_of_active_switches] = col;
                number_of_active_switches++;
            }
        }
    }

    number_of_changes = minimum(number_of_active_switches, 3);

    // This is  being done in such a complex way to guarranty that 3 active switches are found and changed, previous version used to search switches in random
    for (int i = 0; i < number_of_changes;)
    {
        int random_active_sw_index = roll_exclusive(0, number_of_active_switches - 1, previous_changed_sw_index, i);

        const int sampled_sw_active_line_index = active_sw_line[random_active_sw_index];
        const int sampled_sw_active_col_index = active_sw_col[random_active_sw_index];

        int random_pos = roll_exclusive(0, 2, (int *)&(switches[sampled_sw_active_line_index][sampled_sw_active_col_index].position), 1);

        if (switches_verify_possition(switches, sampled_sw_active_line_index, sampled_sw_active_col_index, random_pos) == 1)
        {
            switches[active_sw_line[random_active_sw_index]][active_sw_col[random_active_sw_index]].position = random_pos;
            previous_changed_sw_index[i] = random_active_sw_index;
            i++;
            iteration_counter = 0;
        }
        if (iteration_counter++ > MAX_RANDOM_ATTEMPTS)
        {
            return FAILURE; // cant find a path
        }
    }

    return SUCCESS;
}

bool switches_verify_possition(three_way_switch_t switches[NO_OF_3_WAY_LINES][NO_OF_SWITCHES_PER_LINE], int line, int col, switch_pos_t pos)
{
    // Check edge cases
    if (line != 0)
    {
        if (switches[line - 1][col].position == low_switch && pos == high_switch)
        {
            return false;
        }
    }
    if (line != NO_OF_3_WAY_LINES - 1)
    {
        if (switches[line + 1][col].position == high_switch && pos == low_switch)
        {
            return false; // position not possible
        }
    }
    return true;
}

void switches_connect(three_way_switch_t switches[NO_OF_3_WAY_LINES][NO_OF_SWITCHES_PER_LINE], int start_nodes[NO_OF_3_WAY_LINES], int end_nodes[NO_OF_3_WAY_LINES])
{
    // Connect the switches
    for (int line = 0; line < NO_OF_3_WAY_LINES; line++)
    {
        for (int col = 0; col < NO_OF_SWITCHES_PER_LINE; col++)
        {
            // reset the connections
            for (int i = 0; i < MAX_NUM_SWITCHES_TO_DISTRIBUTE_POWER; i++)
            {
                switches[line][col].connected_to_prev[i] = NULL;
            }
        }
    }

    for (int line = 0; line < NO_OF_3_WAY_LINES; line++)
    {
        for (int col = 0; col < NO_OF_SWITCHES_PER_LINE; col++)
        {

            switch (switches[line][col].position)
            {
            case high_switch:
                switches[line][col].display = '/';
                if (col < NO_OF_SWITCHES_PER_LINE - 1 && line > 0)
                    switches[line - 1][col + 1].connected_to_prev[low_switch] = &switches[line][col];
                break;
            case mid_switch: 
                // to avoid having to create two connections per switch, if the switch on the same life has power, 
                // prioritize it over the low and high switches that might be connected
                switches[line][col].display = '=';
                if (col < NO_OF_SWITCHES_PER_LINE - 1)
                    switches[line][col + 1].connected_to_prev[mid_switch] = &switches[line][col];
                break;
            case low_switch:
                switches[line][col].display = '\\';
                if (col < NO_OF_SWITCHES_PER_LINE - 1 && line < NO_OF_3_WAY_LINES - 1)
                    switches[line + 1][col + 1].connected_to_prev[high_switch] = &switches[line][col];
                break;
            default:
                switches[line][col].display = 'X';
            }
        }
    }
}

void switches_distribute_power(three_way_switch_t switches[NO_OF_3_WAY_LINES][NO_OF_SWITCHES_PER_LINE], int start_nodes[NO_OF_3_WAY_LINES], int end_nodes[NO_OF_3_WAY_LINES])
{

    // reset the end nodes
    resetEndNodes(end_nodes);

    // Distribute power to switches
    for (int i = 0; i < NO_OF_3_WAY_LINES * NO_OF_SWITCHES_PER_LINE; i++)
    {
        for (int line = 0; line < NO_OF_3_WAY_LINES; line++)
        {
            for (int col = 0; col < NO_OF_SWITCHES_PER_LINE; col++)
            {
                switches[line][col].has_power = false;
                if (col == 0 && start_nodes[line] == 1)
                {
                    switches[line][col].has_power = true;
                }
                for (int i = 0; i < MAX_NUM_SWITCHES_TO_DISTRIBUTE_POWER; i++)
                {
                    if (switches[line][col].connected_to_prev[i] != NULL && switches[line][col].connected_to_prev[i]->has_power == true)
                    {
                        switches[line][col].has_power = true;
                    }
                }
                // Distribute power to end nodes
                if (col == NO_OF_SWITCHES_PER_LINE - 1)
                {
                    switch (switches[line][col].position)
                    {
                    case mid_switch:
                        end_nodes[line] |= (switches[line][col].has_power == true);
                        break;
                    case low_switch:
                        if (line < NO_OF_3_WAY_LINES - 1)
                        {
                            end_nodes[line + 1] |= (switches[line][col].has_power == true);
                        }
                        break;
                    case high_switch:
                        if (line > 0)
                        {
                            end_nodes[line - 1] |= (switches[line][col].has_power == true);
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
        }
    }
}

function_status_t switches_control(three_way_switch_t switches[NO_OF_3_WAY_LINES][NO_OF_SWITCHES_PER_LINE], control_index_t *control, rotary_enc_t *rotary, int *button_pushed_flag)
{
    int low_lim = low_switch;
    int high_lim = high_switch;

    // Setup col and line index for user control

    if (rotary->direction == 1) // right
    {
        *button_pushed_flag = 1;

        switches[control->line][control->column].selected = false;
        // clock-wise direction
        if (control->column >= NO_OF_SWITCHES_PER_LINE - 1)
        {
            if (control->line < NO_OF_3_WAY_LINES - 1)
            {
                control->column = 0;
                control->line++;
            }
            else
            {
                control->column = 0;
                control->line = 0;
            }
        }
        else
        {
            control->column++;
        }
    }
    else if (rotary->direction == -1)
    {
        *button_pushed_flag = 1;

        switches[control->line][control->column].selected = false;
        // clock-wise direction
        if (control->column == 0)
        {
            if (control->line > 0)
            {
                control->column = NO_OF_SWITCHES_PER_LINE - 1;
                control->line--;
            }
            else
            {
                control->column = NO_OF_SWITCHES_PER_LINE - 1;
                control->line = NO_OF_3_WAY_LINES - 1;
            }
        }
        else
        {
            control->column--;
        }
    }
    switches[control->line][control->column].selected = true;

    if (rotary->button == 1)
    {
        *button_pushed_flag = 1;

        if (control->line > 0)
        {
            if (switches[control->line - 1][control->column].position == low_switch)
            {
                high_lim = mid_switch;
            }
        }
        if (control->line < NO_OF_3_WAY_LINES - 1)
        {
            if (switches[control->line + 1][control->column].position == high_switch)
            {
                low_lim = mid_switch;
            }
        }
        if (switches[control->line][control->column].position == low_lim)
        {
            switches[control->line][control->column].position = high_lim;
        }
        else
        {
            switches[control->line][control->column].position++;
        }
        // if the switch is red
        const three_way_switch_t *currentSwitch = &switches[control->line][control->column];
        if (currentSwitch->switch_color == red)
        {
            // change the position of the binded switch
            switches[currentSwitch->binded_switch_index.line][currentSwitch->binded_switch_index.column].position = currentSwitch->position;
        }
    }
    return SUCCESS;
}

function_status_t switches_time_calculate(long long current_time, unsigned int max_time_in_ms, unsigned char time_count_active_flag, int *result)
{
    int elapsed_time_ms = current_time - switches_timestamp;
    if (max_time_in_ms <= 0)
    {
        LOG_ERROR("max_time_in_sec and fraction_of_max_time must be positive values.");
        return FAILURE; // Return an error value
    }

    float percentage_elapsed = (elapsed_time_ms / (float)max_time_in_ms) * 10.0f;

    // Clamp the result to the range 0-100
    if (percentage_elapsed <= 1.0f)
    {
        *result = 9;
        return SUCCESS;
    }
    else if (percentage_elapsed >= 10.0f)
    {
        switches_timestamp = current_time;
        *result = 9;
        return SUCCESS;

    }

    *result = (int)(10 - percentage_elapsed);
    return SUCCESS;
}

void switches_time_reset(unsigned long current_time)
{
    switches_timestamp = current_time;
}

int switches_time_get_level_time(int current_level)
{
    return MAX_GAME_TIME_IN_MS - (((MAX_GAME_TIME_IN_MS - MIN_GAME_TIME_IN_MS) / NO_OF_LEVELS) * current_level);
}

int switches_get_level_max_movements(int current_level)
{
    // compute minimum amount of memovements for level
    return MAX_GAME_MOVEMENTS - (((MAX_GAME_MOVEMENTS - MIN_GAME_MOVEMENTS) / NO_OF_LEVELS) * current_level);
}

int switches_get_end_goal()
{
    return end_goal;
}