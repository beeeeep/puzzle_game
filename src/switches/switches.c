#include "switches.h"

#ifdef CMAKE_BUILD
#include "log.h"
#include "red_switches.h"
#include "roll.h"
#else
#include "../log.h"
#include "../roll/roll.h"
#include "red_switches.h"
#endif


#ifdef READ_DIRECTLY_FROM_FILE

#else
#include "data/moves10_final.in"
#include "data/moves1_final.in"
#include "data/moves2_final.in"
#include "data/moves3_final.in"
#include "data/moves4_final.in"
#include "data/moves5_final.in"
#include "data/moves6_final.in"
#include "data/moves7_final.in"
#include "data/moves8_final.in"
#include "data/moves9_final.in"
#endif

// for NUMBER_OF_MAPS_PER_MOVE look the file:  tools/numMovements.txt
#define NUMBER_OF_MAPS_PER_MOVE 48
#define NUM_COLS                5
#define NUM_LINES               5

#define forlines(xx) for (int xx = 0; xx < NUM_LINES; ++xx)
#define forcols(xx)  for (int xx = 0; xx < NUM_COLS; ++xx)
#define forall(xx, yy, kernel) \
    forlines(xx) {             \
        forcols(yy) {          \
            kernel             \
        }                      \
    }

static long long switches_timestamp;

int mod(int a, int b) {
    int r = a % b;
    return r < 0 ? r + b : r;
}

int minimum(int a, int b) {
    return (a < b) ? a : b;
}

void resetEndNodes(int end_nodes[]) {
    memset(end_nodes, 0, sizeof(int) * NO_OF_3_WAY_LINES);
}

char itoc(const int number) {
    return (char) ((number + 0x30) & 0xFF);
}

void default_init_switch(three_way_switches_array_t switches, const int line, const int col);
void default_init_switces(three_way_switches_array_t switches, const int num_lines, const int num_cols);
void init_red_switches(three_way_switches_array_t switches);

void getPossibleSwitchPositions(three_way_switch_t switches[NO_OF_3_WAY_LINES][NO_OF_SWITCHES_PER_LINE], const int line,
    const int column, int possibleSwitchPositions[3]);
int getNextSwitchPosition(int possible_controls[3], const int current_position);
function_status_t pickMapOutOfAll(const int numMovements, map_t* map);
function_status_t pickMapOutOfAllUsingFile(const int numMovements, map_t* map);
function_status_t pickMapOutOfAllUsingArray(const int numMovements, map_t* map);
function_status_t decompressMap(const uint64_t compressedMap, int8_t switches[5][5], int* startx, int* endx);
void init_nodes(int start_nodes[], int end_nodes[], const int star_node_line_index);

function_status_t switches_init(game_state_t* game_state) {
    map_t* map                       = &game_state->map;
    int current_level                = game_state->current_level;
    const int numMovementsPerLevel[] = {1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4};
    // Pick a random path for the current level
    default_init_switces(map->switches, NO_OF_3_WAY_LINES, NO_OF_SWITCHES_PER_LINE);
    init_red_switches(map->switches);
    if (does_fail(pickMapOutOfAll(numMovementsPerLevel[current_level], map))) {
        return FAILURE;
    }
    game_state->movements_left = numMovementsPerLevel[current_level] + 1;
    game_state->time_left      = switches_time_get_level_time(current_level);
    switches_distribute_power(map);
    switches_time_reset(millis_timestamp());
    return SUCCESS;
}

function_status_t pickMapOutOfAll(const int numMovements, map_t* map) {
#ifdef READ_DIRECTLY_FROM_FILE
    return pickMapOutOfAllUsingFile(numMovements, map);
#else
    return pickMapOutOfAllUsingArray(numMovements, map);
#endif
}

function_status_t pickMapOutOfAllUsingFile(const int numMovements, map_t* map) {
    char fileNameBase[]                      = "../tools/moves1_final.in";
    fileNameBase[sizeof(fileNameBase) - 11U] = itoc(numMovements);
    FILE* fIN                                = fopen(fileNameBase, "rb");
    if (fIN == NULL) {
        LOG_ERROR("failed at fopen for file %s", fileNameBase);
        return FAILURE;
    }
    const size_t mapIndex = (size_t) roll(0, NUMBER_OF_MAPS_PER_MOVE - 1);
    LOG_INFO("mapIndex = %lu out of %d", mapIndex, NUMBER_OF_MAPS_PER_MOVE);
    if (fseek(fIN, mapIndex * sizeof(uint64_t), SEEK_SET)) {
        LOG_ERROR("failed at fseek for file %lu", mapIndex * sizeof(uint64_t));
        fclose(fIN);
        return FAILURE;
    }
    uint64_t compressedMap = 0;
    int r                  = fread(&compressedMap, sizeof(uint64_t), 1, fIN);
    LOG_INFO("fread = 0x%lx", compressedMap);
    if (r != 1 || compressedMap == 0) {
        LOG_ERROR("failed at fread[mapIndexSize=%lu] %s", mapIndex * sizeof(uint64_t), fileNameBase);
        fclose(fIN);
        return FAILURE;
    }
    int startx                            = 0;
    int endx                              = 0;
    int8_t positions[NUM_LINES][NUM_COLS] = {0};
    if (does_fail(decompressMap(compressedMap, positions, &startx, &endx))) {
        LOG_ERROR("failed at decompressMap");
        fclose(fIN);
        return FAILURE;
    }
    forall(i, j, { map->switches[i][j].position = positions[i][j]; });

    init_nodes(map->start_nodes, map->end_nodes, startx);
    map->line_end_goal = (int) endx;

    if (fclose(fIN)) {
        fIN = NULL;
        LOG_WARNING("Could not close file");
    }
    fIN = NULL;
    return SUCCESS;
}

function_status_t pickMapOutOfAllUsingArray(const int numMovements, map_t* map) {
    uint64_t compressedMap = 0;
    const size_t mapIndex  = (size_t) roll(0, NUMBER_OF_MAPS_PER_MOVE - 1);
    LOG_INFO("mapIndex = %lu out of %d", mapIndex, NUMBER_OF_MAPS_PER_MOVE);
    switch (numMovements) {
    case 1:
        compressedMap = moves1[mapIndex];
        break;
    case 2:
        compressedMap = moves2[mapIndex];
        break;
    case 3:
        compressedMap = moves3[mapIndex];
        break;
    case 4:
        compressedMap = moves4[mapIndex];
        break;
    case 5:
        compressedMap = moves5[mapIndex];
        break;
    case 6:
        compressedMap = moves6[mapIndex];
        break;
    case 7:
        compressedMap = moves7[mapIndex];
        break;
    case 8:
        compressedMap = moves8[mapIndex];
        break;
    case 9:
        compressedMap = moves9[mapIndex];
        break;
    case 10:
        compressedMap = moves10[mapIndex];
        break;
    default:
        LOG_ERROR("at pickMapOutOfAllUsingArray: numMovements = %d", numMovements);
        return FAILURE;
    }
    int startx                            = 0;
    int endx                              = 0;
    int8_t positions[NUM_LINES][NUM_COLS] = {0};
    if (does_fail(decompressMap(compressedMap, positions, &startx, &endx))) {
        LOG_ERROR("failed at decompressMap");
        return FAILURE;
    }
    forall(i, j, { map->switches[i][j].position = positions[i][j]; });

    init_nodes(map->start_nodes, map->end_nodes, startx);
    map->line_end_goal = (int) endx;
    LOG_INFO("Succesfully picked map");
    return SUCCESS;
}

function_status_t decompressMap(const uint64_t compressedMap, int8_t switches[5][5], int* startx,
    int* endx) // here switches positions have different meaning
{
    int k                    = 0;
    function_status_t status = SUCCESS;
    forall(
        i, j, uint8_t mm = (compressedMap >> k) & 0b11; if (mm == 0) {
            switches[i][j] = mid_switch; // 0
        } else if (mm == 1) {
            switches[i][j] = low_switch; // -1
        } else if (mm == 2) {
            switches[i][j] = high_switch; // 1
        } else {
            LOG_ERROR("mm = %d\n", (int) mm);
            status = FAILURE;
        } k += 2;)* startx = ((compressedMap >> k) & 0b111);
    k += 3;
    *endx = ((compressedMap >> k) & 0b111);
    if (*startx > NUM_LINES) {
        LOG_ERROR("startx = %d", *startx);
        status = FAILURE;
    }
    if (*endx > NUM_LINES) {
        LOG_ERROR("endx = %d", *endx);
        status = FAILURE;
    }
    return status;
}

void init_nodes(int start_nodes[], int end_nodes[], const int star_node_line_index) {
    memset(start_nodes, 0, sizeof(int) * NO_OF_3_WAY_LINES);
    memset(end_nodes, 0, sizeof(int) * NO_OF_3_WAY_LINES);
    start_nodes[star_node_line_index] = 1;
}

void default_init_switces(three_way_switches_array_t switches, const int num_lines, const int num_cols) {
    memset(switches, 0, sizeof(three_way_switch_t) * NO_OF_3_WAY_LINES * NO_OF_SWITCHES_PER_LINE);
    for (int line = 0; line < num_lines; line++) {
        for (int col = 0; col < num_cols; col++) {
            // Initialize the switch
            default_init_switch(switches, line, col);
        }
    }
}

void default_init_switch(three_way_switches_array_t switches, const int line, const int col) {
    three_way_switch_t* sw_init           = &(switches[line][col]);
    sw_init->col                          = col;
    sw_init->line                         = line;
    sw_init->neighbor_switch[high_switch] = NULL;
    sw_init->neighbor_switch[mid_switch]  = NULL;
    sw_init->neighbor_switch[low_switch]  = NULL;
    sw_init->switch_color                 = yellow;
    sw_init->binded_switch_index          = INVALID_CONTROL_INDEX;
    sw_init->has_power                    = false;
    // Connect to top switch
    if (line > 0 && col < NO_OF_SWITCHES_PER_LINE) {
        sw_init->neighbor_switch[high_switch] = &switches[line - 1][col];
    }
    // connect to mid switch void switches_control(three_way_switch_t
    // switches[NO_OF_3_WAY_LINES][NO_OF_SWITCHES_PER_LINE], char **argv)
    if (col < NO_OF_SWITCHES_PER_LINE - 1) {
        sw_init->neighbor_switch[mid_switch] = &switches[line][col + 1];
    }
    // connect to low switch
    if (line < (NO_OF_3_WAY_LINES - 1) && col < NO_OF_SWITCHES_PER_LINE) {
        sw_init->neighbor_switch[low_switch] = &switches[line + 1][col];
    }

    // Top line check
    sw_init->position = (switch_pos_t) roll(high_switch, low_switch);

    if (switches[line - 1][col].position == low_switch && line > 0) {
        sw_init->position = (switch_pos_t) roll(mid_switch, low_switch);
    }
}

void init_red_switches(three_way_switches_array_t switches) {
    int excl_location_list[NUM_COLS]    = {99};
    int random_col;
    int random_line;
    int location_concat;
    int repeat_flag=0;
    
    for (int i = 0; i < NUM_COLS; i++) {

        for (int i = 0; i < NUM_RED_SWITCHES; i++) {
            const int line                   = red_switches_indices[i].line;
            const int col                    = red_switches_indices[i].column;
            switches[line][col].switch_color = red;
            switches[line][col].binded_switch_index.line = red_switches_connections[i].line_b;
            switches[line][col].binded_switch_index.column = red_switches_connections[i].col_b;
            repeat_flag=0;
            switches[line][col].binded_switch_index.line   = random_line;
            switches[line][col].binded_switch_index.column = random_col;
            excl_location_list[i]=location_concat;
    }
}
}
function_status_t switches_randomize_possition(three_way_switch_t switches[NO_OF_3_WAY_LINES][NO_OF_SWITCHES_PER_LINE],
    int end_nodes[NO_OF_3_WAY_LINES], int end_goal) {
    // switch 3 of the critical switches to a random setting
    int previous_changed_sw_index[3] = {0};

    int active_sw_line[NO_OF_3_WAY_LINES]      = {0};
    int active_sw_col[NO_OF_SWITCHES_PER_LINE] = {0};

    int number_of_active_switches = 0;
    int number_of_changes         = 0;
    int iteration_counter         = 0;

    for (int line = 0; line < NO_OF_3_WAY_LINES; line++) {
        for (int col = 0; col < NO_OF_SWITCHES_PER_LINE; col++) {
            if (switches[line][col].has_power == 1) {
                active_sw_line[number_of_active_switches] = line;
                active_sw_col[number_of_active_switches]  = col;
                number_of_active_switches++;
            }
        }
    }

    number_of_changes = minimum(number_of_active_switches, 3);

    // This is  being done in such a complex way to guarranty that 3 active switches are found and changed, previous
    // version used to search switches in random
    for (int i = 0; i < number_of_changes;) {
        int random_active_sw_index = roll_exclusive(0, number_of_active_switches - 1, previous_changed_sw_index, i);

        const int sampled_sw_active_line_index = active_sw_line[random_active_sw_index];
        const int sampled_sw_active_col_index  = active_sw_col[random_active_sw_index];

        int random_pos = roll_exclusive(
            0, 2, (int*) &(switches[sampled_sw_active_line_index][sampled_sw_active_col_index].position), 1);

        if (switches_verify_position(switches, sampled_sw_active_line_index, sampled_sw_active_col_index, random_pos)
            == 1) {
            switches[active_sw_line[random_active_sw_index]][active_sw_col[random_active_sw_index]].position =
                random_pos;
            previous_changed_sw_index[i] = random_active_sw_index;
            i++;
            iteration_counter = 0;
        }
        if (iteration_counter++ > MAX_RANDOM_ATTEMPTS) {
            return FAILURE; // cant find a path
        }
    }

    return SUCCESS;
}

bool switches_verify_position(
    three_way_switch_t switches[NO_OF_3_WAY_LINES][NO_OF_SWITCHES_PER_LINE], int line, int col, switch_pos_t pos) {
    // Check edge cases
    if (line != 0) {
        if (switches[line - 1][col].position == low_switch && pos == high_switch) {
            return false;
        }
    }
    if (line != NO_OF_3_WAY_LINES - 1) {
        if (switches[line + 1][col].position == high_switch && pos == low_switch) {
            return false; // position not possible
        }
    }
    return true;
}


void switches_distribute_power(map_t* map) {
    three_way_switches_array_t* switches = &map->switches;
    int* end_nodes                       = map->end_nodes;
    // reset the end nodes
    resetEndNodes(end_nodes);

    // deactivate all switches
    for (int line = 0; line < NO_OF_3_WAY_LINES; ++line) {
        for (int col = 1; col < NO_OF_SWITCHES_PER_LINE; ++col) {
            (*switches)[line][col].has_power = false;
        }
    }
    // Distribute power to switches
    int lineIndex = -1;
    for (int i = 0; i < NO_OF_3_WAY_LINES; ++i) {
        if (map->start_nodes[i] != 0) {
            lineIndex = i;
            break;
        }
    }

    if (lineIndex == -1) {
        LOG_ERROR("No active node in start_nodes found");
        return;
    }
    (*switches)[lineIndex][0].has_power = true;
    bool has_power                      = true;
    for (int col = 0; col < NO_OF_SWITCHES_PER_LINE - 1; col++) {
        int prevLineIndex = lineIndex;
        switch ((*switches)[lineIndex][col].position) {
        case mid_switch:
            break;
        case low_switch:
            if (lineIndex < NO_OF_3_WAY_LINES - 1) {
                lineIndex++;
            } else {
                LOG_ERROR(
                    "Invalid switch position[%d,%d] %d", lineIndex, col, (int) (*switches)[lineIndex][col].position);
            }
            break;
        case high_switch:
            if (lineIndex > 0) {
                lineIndex--;
            } else {
                LOG_ERROR(
                    "Invalid switch position[%d,%d] %d", lineIndex, col, (int) (*switches)[lineIndex][col].position);
            }
            break;
        default:
            LOG_WARNING("Invalid switch position %d", (int) (*switches)[lineIndex][col].position);
            break;
        }
        has_power                                 = (*switches)[prevLineIndex][col].has_power;
        (*switches)[lineIndex][col + 1].has_power = has_power;
    }
    const int lastCollumn = NO_OF_SWITCHES_PER_LINE - 1;
    switch ((*switches)[lineIndex][lastCollumn].position) // for the final switch of the line
    {
    case mid_switch:
        break;
    case low_switch:
        if (lineIndex < NO_OF_3_WAY_LINES - 1) {
            lineIndex++;
        } else {
            LOG_ERROR("Invalid switch position[%d,%d] %d", lineIndex, lastCollumn,
                (int) (*switches)[lineIndex][lastCollumn].position);
        }
        break;
    case high_switch:
        if (lineIndex > 0) {
            lineIndex--;
        } else {
            LOG_ERROR("Invalid switch position[%d,%d] %d", lineIndex, lastCollumn,
                (int) (*switches)[lineIndex][lastCollumn].position);
        }
        break;
    default:
        LOG_WARNING("Invalid switch position %d", (int) (*switches)[lineIndex][lastCollumn].position);
        break;
    }
    end_nodes[lineIndex] = 1; // Power always reaches the final column
    // LOG_INFO("Finished distributing power");
}

function_status_t switches_control(game_state_t* game_state, int* button_pushed_flag) {
    three_way_switches_array_t* switches                 = &game_state->map.switches;
    control_index_t* control                             = &game_state->control;
    rotary_enc_t* rotary                                 = &game_state->rotary;
    (*switches)[control->line][control->column].selected = false;
    // Setup col and line index for user control
    const int diff = control->column + rotary->direction;
    static int switch_moved;
    static int line_prev;
    static int col_prev;
    if (rotary->direction == 1) {
        *button_pushed_flag = 1;
        control->column     = mod(diff, NO_OF_SWITCHES_PER_LINE);
        control->line += diff / NO_OF_SWITCHES_PER_LINE;
        control->line = control->line % NO_OF_3_WAY_LINES;
    } else if (rotary->direction == -1) {
        *button_pushed_flag = 1;
        control->column     = mod(diff, NO_OF_SWITCHES_PER_LINE);
        control->line -= (diff < 0);
        control->line = mod(control->line, NO_OF_3_WAY_LINES);
    }

    three_way_switch_t* currentSwitch = &(*switches)[control->line][control->column];
    currentSwitch->selected           = true;

    if (rotary->button == 1) // if about to change position
    {
        *button_pushed_flag = 1;
        int possibleSwitchPositions[3];
        getPossibleSwitchPositions((*switches), control->line, control->column, possibleSwitchPositions);

        // if the switch is red
        if (currentSwitch->switch_color == red) {
            // change the position of the binded switch if able
            int possibleRedSwitchPositions[3];
            getPossibleSwitchPositions((*switches), currentSwitch->binded_switch_index.line,
                currentSwitch->binded_switch_index.column, possibleRedSwitchPositions);
            possibleSwitchPositions[0] &= possibleRedSwitchPositions[0];
            possibleSwitchPositions[1] &= possibleRedSwitchPositions[1];
            possibleSwitchPositions[2] &= possibleRedSwitchPositions[2];
        }
        currentSwitch->position = getNextSwitchPosition(possibleSwitchPositions, currentSwitch->position);
        if (currentSwitch->switch_color == red) {
            // change the position of the binded switch to accordinate to the red switch
            three_way_switch_t* bindedSwitch = &(*switches)[currentSwitch->binded_switch_index.line][currentSwitch->binded_switch_index.column];
            bindedSwitch->position = currentSwitch->position;
        }
        if (possibleSwitchPositions[0] != 0 || possibleSwitchPositions[2] != 0) {
            switch_moved = 1;
            line_prev    = control->line;
            col_prev     = control->column;
            game_state->movements_left--;
        }
    }
    return SUCCESS;
}

void getPossibleSwitchPositions(three_way_switch_t switches[NO_OF_3_WAY_LINES][NO_OF_SWITCHES_PER_LINE], const int line,
    const int column, int possibleSwitchPositions[3]) {
    possibleSwitchPositions[0] = 0;
    possibleSwitchPositions[1] = 1;
    possibleSwitchPositions[2] = 0;

    if (line > 0 && switches[line - 1][column].position != low_switch) {
        possibleSwitchPositions[0] = 1;
    }
    if (line < NO_OF_3_WAY_LINES - 1 && switches[line + 1][column].position != high_switch) {
        possibleSwitchPositions[2] = 1;
    }
}

int getNextSwitchPosition(int possibleSwitchPositions[3], const int current_position) {
    // assume possibleSwitchPositions always has possibleSwitchPositions[1] = 1
    int nextSwitchPosition = (current_position + 1) % 3;
    while (possibleSwitchPositions[nextSwitchPosition] != 1) {
        nextSwitchPosition = (nextSwitchPosition + 1) % 3;
    }
    return nextSwitchPosition;
}

function_status_t switches_time_calculate(
    long long current_time, unsigned int max_time_in_ms, unsigned char time_count_active_flag, int* result) {

    int elapsed_time_ms = current_time - switches_timestamp;
    if (max_time_in_ms <= 0) {
        LOG_ERROR("max_time_in_sec and fraction_of_max_time must be positive values.");
        return FAILURE; // Return an error value
    }
    *result = (max_time_in_ms - elapsed_time_ms);
    return SUCCESS;
}

void switches_time_reset(unsigned long current_time) {
    switches_timestamp = current_time;
}

int switches_time_get_level_time(int current_level) {
    return MAX_GAME_TIME_IN_MS; //- (((MAX_GAME_TIME_IN_MS - MIN_GAME_TIME_IN_MS) / NO_OF_LEVELS) * current_level);
}

int switches_get_level_max_movements(int current_level) {
    // compute minimum amount of memovements for level
    return MAX_GAME_MOVEMENTS - (((MAX_GAME_MOVEMENTS - MIN_GAME_MOVEMENTS) / NO_OF_LEVELS) * current_level);
}
