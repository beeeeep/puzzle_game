#include "mui.h"

#include "leds_init.h"
#include "misc.h"
#include "rotary/rotary.h"
#include "servo_controller/servo_controller.h"
#include "switches.h"
#include "time_display/time_display.h"
#include <PCA9685.h>
#include <Wire.h>


#define ROTARY_A_PIN      10
#define ROTARY_B_PIN      11
#define ROTARY_BUTTON_PIN 12
#define SCL_PIN           4
#define SDA_PIN           5
#define PCA9685_OUTPUT_ENABLE_PIN
#define NIXIE_LATCH_PIN 8
#define NIXIE_CLOCK_PIN 3
#define NIXIE_SER_PIN   18


PCA9685 pca9685;
servo_motor_t servos[NO_OF_SERVOS];
leds_ctrl_str_t leds;
rotary_t rotary;

bool change_position_flag = false;
control_index_t control_position;

int nixie_data[16] = {64, 32, 16, 8, 4, 2, 1, 32768, 16384, 8192};

void nixie_set_number(unsigned char number) {
    shiftOut(NIXIE_SER_PIN, NIXIE_CLOCK_PIN, MSBFIRST, 0xFF & (nixie_data[number] >> 8));
    shiftOut(NIXIE_SER_PIN, NIXIE_CLOCK_PIN, MSBFIRST, 0xFF & nixie_data[number]);
}

void nixie_power_off(void) {
    shiftOut(NIXIE_SER_PIN, NIXIE_CLOCK_PIN, MSBFIRST, 0xFF);
    shiftOut(NIXIE_SER_PIN, NIXIE_CLOCK_PIN, MSBFIRST, 0xFF);
}

void setDeviceChannelServoPulseDuration_wrapper(
    unsigned char cntl_addr, unsigned char servo_channel, unsigned int pos) {
    pca9685.setDeviceChannelServoPulseDuration(cntl_addr, servo_channel, pos);
}

void initVisuals() {
    init_servos();
    initializeLEDs(leds);
    nixie_controller_init(nixie_set_number, nixie_power_off, millis);
    time_display_init(setDeviceChannelServoPulseDuration_wrapper, 0x40, 0);
}

void init_servos() {
    // Init Servos, these are not placed in a for loop so its easier to calibrate then idividually
    /*0-4*/
    servo_ctrl_init(millis_wrapper, setDeviceChannelServoPulseDuration_wrapper);
    servo_ctrl_add_device(servos, 0, 0x40, 0, 2000, 1500, 800);
    servo_ctrl_add_device(servos, 1, 0x40, 1, 2000, 1500, 800);
    servo_ctrl_add_device(servos, 2, 0x40, 2, 2000, 1500, 800);
    servo_ctrl_add_device(servos, 3, 0x40, 3, 2000, 1500, 800);
    servo_ctrl_add_device(servos, 4, 0x40, 4, 2000, 1500, 800);
    /*5-9*/
    servo_ctrl_add_device(servos, 5, 0x40, 5, 2000, 1500, 800);
    servo_ctrl_add_device(servos, 6, 0x40, 6, 2000, 1500, 800);
    servo_ctrl_add_device(servos, 7, 0x40, 7, 2000, 1500, 800);
    servo_ctrl_add_device(servos, 8, 0x40, 8, 2000, 1500, 800);
    servo_ctrl_add_device(servos, 9, 0x40, 9, 2000, 1500, 800);
    /*10-14*/
    servo_ctrl_add_device(servos, 10, 0x40, 10, 2000, 1500, 800);
    servo_ctrl_add_device(servos, 11, 0x40, 11, 2000, 1500, 800);
    servo_ctrl_add_device(servos, 12, 0x40, 12, 2000, 1500, 800);
    servo_ctrl_add_device(servos, 13, 0x40, 13, 2000, 1500, 800);
    servo_ctrl_add_device(servos, 14, 0x40, 14, 2000, 1500, 800);
    /*15-19*/
    servo_ctrl_add_device(servos, 15, 0x40, 15, 2000, 1500, 800);
    servo_ctrl_add_device(servos, 16, 0x41, 0, 2000, 1500, 800);
    servo_ctrl_add_device(servos, 17, 0x41, 1, 2000, 1500, 800);
    servo_ctrl_add_device(servos, 18, 0x41, 2, 2000, 1500, 800);
    servo_ctrl_add_device(servos, 19, 0x41, 3, 2000, 1500, 800);
    /*20-24*/
    servo_ctrl_add_device(servos, 20, 0x41, 4, 2000, 1500, 800);
    servo_ctrl_add_device(servos, 21, 0x41, 5, 2000, 1500, 800);
    servo_ctrl_add_device(servos, 22, 0x41, 6, 2000, 1500, 800);
    servo_ctrl_add_device(servos, 23, 0x41, 7, 2000, 1500, 800);
    servo_ctrl_add_device(servos, 24, 0x41, 8, 2000, 1500, 800);
    servo_ctrl_update(servos);
}

unsigned long millis_wrapper(void) {
    return (unsigned long) millis();
}

unsigned char rotary_read_pin_A(void) {
    return (unsigned char) digitalRead(ROTARY_A_PIN);
}

unsigned char rotary_read_pin_B(void) {
    return (unsigned char) digitalRead(ROTARY_B_PIN);
}

unsigned char rotary_read_button(void) {
    return (unsigned char) digitalRead(ROTARY_BUTTON_PIN);
}

void initRotaryEncoder() {
    pinMode(ROTARY_A_PIN, INPUT_PULLUP);
    pinMode(ROTARY_B_PIN, INPUT_PULLUP);
    pinMode(ROTARY_BUTTON_PIN, INPUT_PULLUP);
    attachInterrupt(ROTARY_A_PIN, Rotary_AB_pin_callback_fuction, FALLING);
    attachInterrupt(ROTARY_B_PIN, Rotary_AB_pin_callback_fuction, FALLING);
    attachInterrupt(ROTARY_BUTTON_PIN, Rotary_button_pin_callback_fuction, FALLING);
    Rotary_init(rotary_read_pin_A, rotary_read_pin_B, rotary_read_button, millis_wrapper);
}

// WRAPPER FUNCTIONS
void setDeviceChannelServoPulseDuration_wrapper(
    unsigned char cntl_addr, unsigned char servo_channel, unsigned int pos) {
    pca9685.setDeviceChannelServoPulseDuration(cntl_addr, servo_channel, pos);
}

servo_pos_t switches_pos_to_servo_pos(switch_pos_t pos) {
    switch (pos) {
    case high_switch:
        return servo_pos_high;
    case mid_switch:
        return servo_pos_center;
    case low_switch:
        return servo_pos_low;
    default:
        return undefined;
    }
}

void drawLevel(map_t* map) {
    static long flash_on_timestamp;
    static int flash_flag;
    int activeSegments[5][6] = {0};
    markActiveSegments(activeSegments, map->switches, start_line);
    int start_line = -1;
    for (int i = 0; i < NO_OF_3_WAY_LINES; ++i) {
        if (start_nodes[i] != 0) {
            start_line = i;
        }
    }

    // draw the leds
    if (millis_timestamp() - flash_on_timestamp > 200) {
        flash_on_timestamp = millis_timestamp();
        (flash_flag == 0) ? (flash_flag = 1) : (flash_flag = 0);
    }
    for (int lineIndex = 0; lineIndex < NO_OF_STATIC_LEDBARS_LINES; ++lineIndex) {
        for (int columnIndex = 0; columnIndex < NO_OF_STATIC_LEDBARS_PER_LINE; ++columnIndex) {
            leds.ledbar_static[columnIndex][lineIndex].state_prev = leds.ledbar_static[columnIndex][lineIndex].state;
            leds.ledbar_static[columnIndex][lineIndex].state =
                (activeSegments[lineIndex][columnIndex] != 0) ? lamp_state_on : lamp_state_off;
        }
        for (int columnIndex = 0; columnIndex < NO_OF_SWITCH_LEDS_PER_LINE; ++columnIndex) {
            if ((map->switches[lineIndex][columnIndex].selected)) {
                if (change_position_flag) {
                    servo_pos_t prev_servo_position = servos[lineIndex * 5 + columnIndex].position;
                    servos[lineIndex * 5 + columnIndex].position =
                        switches_pos_to_servo_pos(map->switches[lineIndex][columnIndex].position);
                    if (prev_servo_position != servos[lineIndex * 5 + columnIndex].position) {
                        servo_ctrl_change_position(servos[control_position.line * 5 + control_position.column]);
                    }
                    change_position_flag = false;
                }

                leds.ledbar_switch[columnIndex][lineIndex].state_prev =
                    (map->switches[lineIndex][columnIndex].has_power) ? lamp_state_on : lamp_state_off;
                leds.ledbar_switch[columnIndex][lineIndex].state = lamp_state_blink;
            } else {
                leds.ledbar_switch[columnIndex][lineIndex].state =
                    (map->switches[lineIndex][columnIndex].has_power) ? lamp_state_on : lamp_state_off;
            }
        }
        leds.led_lamp[0][lineIndex] = (map->start_nodes[lineIndex] != 0) ? lamp_state_on : lamp_state_off;
        leds.led_lamp[1][lineIndex] = (activeSegments[lineIndex][5] != 0) ? lamp_state_on : lamp_state_off;
    }
    // at rotary encoder position make it blinking
    led_controller_update(&leds);
}

void appendInfo(const int end_goal, const int time_left, const int current_level) {
    unsigned int max_time = switches_time_get_level_time(current_level);
    time_display_set_time((unsigned int) time_left, max_time);
    nixie_controller_diplay_number(end_goal);
}

void get_controls_status(rotary_enc_t* rotary) {
    Rotary_get_status(rotary);
    if (rotary->button) {
        change_position_flag = true;
    }
}

void shutDownDevice() {
    // YOLO (literaly): No shutdown procedure
}

void init_gui_structures(userInterface_t** gui) {
    pca9685.setDeviceChannelServoPulseDuration(0x40, 0, 16000);
    leds_setPca9685(&pca9685);
    led_controller_test(&leds);
    *gui                        = (userInterface_t*) malloc(sizeof(userInterface_t));
    (*gui)->initVisuals         = initVisuals;
    (*gui)->initControls        = initRotaryEncoder;
    (*gui)->drawLevel           = drawLevel;
    (*gui)->appendInfo          = appendInfo;
    (*gui)->get_controls_status = Rotary_get_status;
    (*gui)->terminate           = shutDownDevice;
}

void delete_gui_structures(userInterface_t** gui) {
    free(*gui);
    *gui = NULL;
}
