#include "Rotary.h"

static rotary_t rotary_status;
static unsigned int hold_timestamp              = 0;
static volatile button_state_t button_state = BUTTON_RELEASED;

static Get_rotary_pin_state_t get_rotary_A_pin_state;
static Get_rotary_pin_state_t get_rotary_B_pin_state;
static Get_rotary_pin_state_t get_rotary_button_pin_state;
milliseconds_t milliseconds;

void Rotary_init(Get_rotary_pin_state_t get_A_pin_state, Get_rotary_pin_state_t get_B_pin_state,
    Get_rotary_pin_state_t get_button_pin_state, milliseconds_t millis_wrapper) {
    get_rotary_A_pin_state      = get_A_pin_state;
    get_rotary_B_pin_state      = get_B_pin_state;
    get_rotary_button_pin_state = get_button_pin_state;
    milliseconds                      = millis_wrapper;
}

void Rotary_get_status(rotary_t* status) {
    static unsigned char button_read_flag = 0;
    status->direction                     = rotary_status.direction;
    rotary_status.direction               = 0;
    Rotary_button_pin_callback_fuction();
    if (button_state == BUTTON_HOLDING_DOWN && button_read_flag == 0) {
        status->button                 = 1;
        status->button_hold_time_in_ms = (milliseconds() - hold_timestamp);
        button_read_flag               = 1;
    } else if (button_state == BUTTON_HOLDING_DOWN) {
        status->button_hold_time_in_ms = (milliseconds() - hold_timestamp);
        status->button                 = 0;
    } else {
        status->button                 = 0;
        status->button_hold_time_in_ms = 0;
        button_read_flag               = 0;
    }
}

void Rotary_AB_pin_callback_fuction(void) {
    static unsigned char rotary_state = 0;
    unsigned char tempState           = get_rotary_B_pin_state() | (get_rotary_A_pin_state() << 1);

    if (tempState == 3) {
        rotary_state = tempState;
    } else if (rotary_state == 2) // TURN LEFT
    {
        if (tempState == 1) {
            rotary_state            = 0;
            rotary_status.direction = -1;
        }
    } else if (rotary_state == 1) // TURN RIGHT
    {
        if (tempState == 2) {
            rotary_state            = 0;
            rotary_status.direction = 1;
        }
    } else {
        rotary_state = tempState;
    }
}

void Rotary_button_pin_callback_fuction(void) {
    unsigned char buttonpin_state         = get_rotary_button_pin_state();
    static volatile unsigned int timestamp_on = 0;

    switch (button_state) {
    case BUTTON_RELEASED:
        if (buttonpin_state == 0 && milliseconds() - timestamp_on > 20) {
            button_state   = BUTTON_HOLDING_DOWN;
            hold_timestamp = milliseconds();
        }
        break;
    case BUTTON_HOLDING_DOWN:
        if (buttonpin_state == 1) {
            button_state = BUTTON_RELEASED;
            timestamp_on = milliseconds();
        }
        break;
    default:
        break;
    }
}
