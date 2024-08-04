#include "switches.h"

int switches_init(three_way_switch_t switches[NO_OF_3_WAY_LINES][NO_OF_SWITCHES_PER_LINE], int start_nodes[NO_OF_3_WAY_LINES], int end_nodes[NO_OF_3_WAY_LINES], int end_goal)
{
    unsigned long path_tracing_attemtps = 0;
    int path_found_counter = 0;
    int random_number_of_paths_found = roll(0, 99);
    int start_node_index = roll(0, NO_OF_3_WAY_LINES - 1);

    //Reset all the nodes and switches
    memset(start_nodes, 0, sizeof(int) * NO_OF_3_WAY_LINES);
    memset(end_nodes, 0, sizeof(int) * NO_OF_3_WAY_LINES);

    for (int i = 0; i < NO_OF_3_WAY_LINES; i++)
    {
        start_nodes[i] = 0;
        end_nodes[i] = 0;
    }
    start_nodes[start_node_index] = 1;

    //Find a random path for the end goal
    do
    {
        path_tracing_attemtps++;
        memset(switches, 0, sizeof(three_way_switch_t) * NO_OF_3_WAY_LINES * NO_OF_SWITCHES_PER_LINE);

        //   Initialize the switches
        for (int line = 0; line < NO_OF_3_WAY_LINES; line++)
        {
            for (int col = 0; col < NO_OF_SWITCHES_PER_LINE; col++)
            {
                switches[line][col].col = col;
                switches[line][col].line = line;
                switches[line][col].neighbor_switch[high_switch] = NULL;
                switches[line][col].neighbor_switch[mid_switch] = NULL;
                switches[line][col].neighbor_switch[low_switch] = NULL;

                // Connect to top switch
                if (line > 0 && col < NO_OF_SWITCHES_PER_LINE)
                {
                    switches[line][col].neighbor_switch[high_switch] = &switches[line - 1][col];
                }
                // connect to mid switch void switches_control(three_way_switch_t switches[NO_OF_3_WAY_LINES][NO_OF_SWITCHES_PER_LINE], char **argv)
                if (col < NO_OF_SWITCHES_PER_LINE - 1)
                {
                    switches[line][col].neighbor_switch[mid_switch] = &switches[line][col + 1];
                }
                // connect to low switch
                if (line < (NO_OF_3_WAY_LINES - 1) && col < NO_OF_SWITCHES_PER_LINE)
                {
                    switches[line][col].neighbor_switch[low_switch] = &switches[line + 1][col];
                }

                // Top line check
                switches[line][col].possition = (switch_pos_t)roll(high_switch, low_switch);

                if (switches[line - 1][col].possition == low_switch && line > 0)
                {
                    switches[line][col].possition = (switch_pos_t)roll(mid_switch, low_switch);
                }
            }
        }

        switches_connect(switches, start_nodes, end_nodes, end_goal);
        switches_distribute_power(switches, start_nodes, end_nodes);

        if (end_nodes[end_goal] == 1)
        {
            path_found_counter++;
            path_tracing_attemtps = 0;
        }
        else if (path_tracing_attemtps >= MAX_RANDOM_ATTEMPTS) // if this is true, it means that the path tracing is stuck
        {
#ifdef ERROR_REPORT
            fprintf(stderr, "ERROR: path tracing stuck");
#endif                // ERROR_REPORT
            return 0; // return error
        }
    } while (path_found_counter < random_number_of_paths_found);

    // Randomize the active switches
    while (end_nodes[end_goal] == 1)
    {
        if (switches_randomize_possition(switches, end_nodes, end_goal) == 0)
        {
#ifdef ERROR_REPORT
            fprintf(stderr, "ERROR: switch randomizing stuck");
#endif                // ERROR_REPORT
            return 0; // return error
        }
        // Run this again to redistribute power after randomizing
        switches_connect(switches, start_nodes, end_nodes, end_goal);
        switches_distribute_power(switches, start_nodes, end_nodes);
    }

    return 1;
}

int switches_randomize_possition(three_way_switch_t switches[NO_OF_3_WAY_LINES][NO_OF_SWITCHES_PER_LINE], int end_nodes[NO_OF_3_WAY_LINES], int end_goal)
{
    // switch 3 of the critical switches to a random setting
    int previous_changed_sw_line_index[3] = {0};
    int previous_changed_sw_col_index[3] = {0};
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

    number_of_changes = (number_of_active_switches < 3) ? number_of_active_switches : 3;

    // This is  being done in such a complex way to guarranty that 3 active switches are found and changed, previous version used to search switches in random
    for (int i = 0; i < number_of_changes; i)
    {
        int random_line_index = roll_exclusive(0, number_of_active_switches - 1, previous_changed_sw_line_index, i);
        int random_col_index = roll_exclusive(0, number_of_active_switches - 1, previous_changed_sw_col_index, i);

        int random_pos = roll_exclusive(0, 2, (int *)&(switches[active_sw_line[random_line_index]][active_sw_col[random_col_index]].possition), 1);

        if (switches_verify_possition(switches, active_sw_line[random_line_index], active_sw_col[random_col_index], random_pos) == 1)
        {
            switches[active_sw_line[random_line_index]][active_sw_col[random_col_index]].possition = random_pos;
            previous_changed_sw_line_index[i] = random_line_index;
            previous_changed_sw_col_index[i] = random_col_index;
            i++;
            iteration_counter = 0;
        }
        if (iteration_counter++ > MAX_RANDOM_ATTEMPTS)
        {
            return 0; //cant find a path
        }
    }

    return 1;
}

int switches_verify_possition(three_way_switch_t switches[NO_OF_3_WAY_LINES][NO_OF_SWITCHES_PER_LINE], int line, int col, switch_pos_t pos)
{
    // Check edge cases
    if (line != 0)
    {
        if (switches[line - 1][col].possition == low_switch && pos == high_switch)
        {
            return 0;
        }
    }
    if (line != NO_OF_3_WAY_LINES - 1)
    {
        if (switches[line + 1][col].possition == high_switch && pos == low_switch)
        {
            return 0; //position not possible
        }
    }
    return 1;
}

void switches_connect(three_way_switch_t switches[NO_OF_3_WAY_LINES][NO_OF_SWITCHES_PER_LINE], int start_nodes[NO_OF_3_WAY_LINES], int end_nodes[NO_OF_3_WAY_LINES], int end_goal)
{
    // Connect the switches
    for (int line = 0; line < NO_OF_3_WAY_LINES; line++)
    {
        for (int col = 0; col < NO_OF_SWITCHES_PER_LINE; col++)
        {
            // reset the connections
            for (int i = 0; i < 3; i++)
            {
                switches[line][col].connected_to_prev[i] = NULL;
            }
        }
    }

    for (int line = 0; line < NO_OF_3_WAY_LINES; line++)
    {
        for (int col = 0; col < NO_OF_SWITCHES_PER_LINE; col++)
        {

            switch (switches[line][col].possition)
            {
            case high_switch:

                switches[line][col].display = '/';
                if (col < NO_OF_SWITCHES_PER_LINE - 1 && line > 0)
                    switches[line - 1][col + 1].connected_to_prev[low_switch] = &switches[line][col];
                break;
            case mid_switch: // to avoid having to create two connections per switch, if the switch on the same life has power, prioritize it over the low and high switches that might be connected
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
    for (int i = 0; i < NO_OF_3_WAY_LINES; i++)
    {
        end_nodes[i] = 0;
    }

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
                for (int i = 0; i < 3; i++)
                {
                    if (switches[line][col].connected_to_prev[i] != NULL && switches[line][col].connected_to_prev[i]->has_power == true)
                    {
                        switches[line][col].has_power = true;
                    }
                }
                // Distribute power to end nodes
                if (col == NO_OF_SWITCHES_PER_LINE - 1)
                {
                    switch (switches[line][col].possition)
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
int switches_control(three_way_switch_t switches[NO_OF_3_WAY_LINES][NO_OF_SWITCHES_PER_LINE], control_index_t *control, rotary_enc_t *rotary, int *button_pushed_flag)
{

    static long input_timestamp;
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
            if (switches[control->line - 1][control->column].possition == low_switch)
            {
                high_lim = mid_switch;
            }
        }
        if (control->line < NO_OF_3_WAY_LINES - 1)
        {
            if (switches[control->line + 1][control->column].possition == high_switch)
            {
                low_lim = mid_switch;
            }
        }
        (switches[control->line][control->column].possition == low_lim) ? (switches[control->line][control->column].possition = high_lim) : (switches[control->line][control->column].possition++);
    }
}

static long long switches_timestamp;

int switches_time_calculate(long long current_time, unsigned int max_time_in_ms, unsigned char time_count_active_flag)
{
    int elapsed_time_ms = current_time - switches_timestamp;
    if (max_time_in_ms <= 0)
    {
#ifdef ERROR_REPORT
        fprintf(stderr, "Error: max_time_in_sec and fraction_of_max_time must be positive values.\n");
#endif            // ERROR_REPORT
        return 0; // Return an error value
    }

    float percentage_elapsed = (elapsed_time_ms / (float)max_time_in_ms) * 10.0f;

    // Clamp the result to the range 0-100
    if (percentage_elapsed <= 1.0f)
    {
        return 9;
    }
    else if (percentage_elapsed >= 10.0f)
    {
        switches_timestamp = current_time;
        return 9;
    }

    return (int)(10 - percentage_elapsed);
}

void switches_time_reset(unsigned long current_time)
{
    switches_timestamp = current_time;
}

int switches_time_get_level_time(int current_level)
{
    return MAX_GAME_TIME_IN_MS - (((MAX_GAME_TIME_IN_MS - MIN__GAME_TIME_IN_MS) / NO_OF_LEVELS) * current_level);
}

