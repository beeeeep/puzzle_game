#include "mui.h"

#include "src/misc.h"

#include "src/led_controller/led_controller.h"
#include "src/nixie_controller/nixie_controller.h"
#include "src/rotary/rotary.h"
#include "src/servo_controller/servo_controller.h"
#include "src/switches/switches.h"
#include "src/time_display/time_display.h"
#include "src/sound/sound_module2.h"
#include <stdlib.h>

#define ROTARY_A_PIN      10
#define ROTARY_B_PIN      11
#define ROTARY_BUTTON_PIN 12
#define SCL_PIN           4
#define SDA_PIN           5
#define PCA9685_OUTPUT_ENABLE_PIN
#define NIXIE_LATCH_PIN 8
#define NIXIE_CLOCK_PIN 3
#define NIXIE_SER_PIN   18

#define LED_BAR_STATIC_SER_RCLK 16
#define LED_BAR_STATIC_SER_SCLK 17
#define LED_BAR_STATIC_SER_OE   8
#define LED_BAR_STATIC_SER_0    7
#define LED_BAR_STATIC_SER_1    5
#define LED_BAR_STATIC_SER_2    18
#define LED_BAR_STATIC_SER_3    15

#define LED_LAMP_SCLK 47
#define LED_LAMP_RCLK 21
#define LED_LAMP_OE   48
#define LED_LAMP_SER  15


#define SOUND_MODULE_PIN_S1  0
#define SOUND_MODULE_PIN_S2  1
#define SOUND_MODULE_PIN_S3  2
#define SOUND_MODULE_PIN_S6  3

PCA9685 pca9685;
servo_motor_t servos[NO_OF_SERVOS];
leds_ctrl_str_t leds;
rotary_t rotary;
GPD2846 sound_module(SOUND_MODULE_PIN_S1, SOUND_MODULE_PIN_S2, SOUND_MODULE_PIN_S3, SOUND_MODULE_PIN_S6, 20);

bool change_position_flag = false;
control_index_t control_position;

int nixie_data[16]             = {64, 32, 16, 8, 4, 2, 1, 32768, 16384, 8192};
const int led_lamp_offsets[16] = {64, 32, 16, 8, 4, 2, 1, 32768, 16384, 8192};
const int ledbar_offsets[4][8] = {{
                                      64,
                                      32,
                                      16,
                                      8,
                                      4,
                                      2,
                                      1,
                                      32768,
                                  },
    {64, 32, 16, 8, 4, 2, 1, 32768}, {64, 32, 16, 8, 4, 2, 1, 32768}, {64, 32, 16, 8, 4, 2, 1, 32768}};

// WRAPPER FUNCTIONS
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

void led_bar_set_pwm_duty_cycle(unsigned char device_ID, unsigned char channel, unsigned int power) {
    pca9685.setDeviceChannelDutyCycle(device_ID, channel, power);
}

void led_bar_static_set_state(unsigned char device_no, unsigned char channel, unsigned char state) {
    unsigned char ser_pin = 0;
    static unsigned int led_bar_data_reg[4];
    switch (device_no) {
    case 0:
        ser_pin = LED_BAR_STATIC_SER_0;
        break;
    case 1:
        ser_pin = LED_BAR_STATIC_SER_1;
        break;
    case 2:
        ser_pin = LED_BAR_STATIC_SER_2;
        break;
    case 3:
        ser_pin = LED_BAR_STATIC_SER_3;
        break;
    }
    if (state == 1) {
        led_bar_data_reg[device_no] |= (1 << ledbar_offsets[device_no][channel]); // set to high
    } else {
        led_bar_data_reg[device_no] &= ~(1 << ledbar_offsets[device_no][channel]); // set to low
    }
    shiftOut(ser_pin, LED_LAMP_SCLK, MSBFIRST, 0xFF & (led_bar_data_reg[device_no] >> 8));
    shiftOut(ser_pin, LED_LAMP_SCLK, MSBFIRST, 0xFF & led_bar_data_reg[device_no]);
}

void led_lamp_set_state(unsigned char channel, unsigned char state) {
    static unsigned int led_lamp_data_reg;

    if (state == 1) {
        led_lamp_data_reg |= (1 << led_lamp_offsets[channel]); // set to high
    } else {
        led_lamp_data_reg &= ~(1 << led_lamp_offsets[channel]); // set to low
    }

    digitalWrite(LED_LAMP_RCLK, 0);
    shiftOut(LED_LAMP_SER, LED_LAMP_SCLK, MSBFIRST, 0xFF & (led_lamp_data_reg >> 8));
    shiftOut(LED_LAMP_SER, LED_LAMP_SCLK, MSBFIRST, 0xFF & led_lamp_data_reg);
    digitalWrite(LED_LAMP_SCLK, 1);
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

void initializeLEDs(leds_ctrl_str_t* leds) {
    led_controller_init(millis_wrapper, led_bar_set_pwm_duty_cycle, led_bar_static_set_state, led_lamp_set_state, leds);
    digitalWrite(LED_LAMP_OE, 0);
    digitalWrite(LED_BAR_STATIC_SER_OE, 0);

    // Initialize the led bards that are on the switches, not placed in a for loop so they can be calibrated
    // idividually
    /*0-4*/
    led_controller_add_led_switch_device(&leds->ledbar_switch[0][0], 0x42, 0);
    led_controller_add_led_switch_device(&leds->ledbar_switch[0][1], 0x42, 1);
    led_controller_add_led_switch_device(&leds->ledbar_switch[0][2], 0x42, 2);
    led_controller_add_led_switch_device(&leds->ledbar_switch[0][3], 0x42, 3);
    led_controller_add_led_switch_device(&leds->ledbar_switch[0][4], 0x42, 4);
    /*5-9*/
    led_controller_add_led_switch_device(&leds->ledbar_switch[1][0], 0x42, 5);
    led_controller_add_led_switch_device(&leds->ledbar_switch[1][1], 0x42, 6);
    led_controller_add_led_switch_device(&leds->ledbar_switch[1][2], 0x42, 7);
    led_controller_add_led_switch_device(&leds->ledbar_switch[1][3], 0x42, 8);
    led_controller_add_led_switch_device(&leds->ledbar_switch[1][4], 0x42, 9);
    /*10-14*/
    led_controller_add_led_switch_device(&leds->ledbar_switch[2][0], 0x42, 10);
    led_controller_add_led_switch_device(&leds->ledbar_switch[2][1], 0x42, 11);
    led_controller_add_led_switch_device(&leds->ledbar_switch[2][2], 0x42, 12);
    led_controller_add_led_switch_device(&leds->ledbar_switch[2][3], 0x42, 13);
    led_controller_add_led_switch_device(&leds->ledbar_switch[2][4], 0x42, 14);
    /*15-19*/
    led_controller_add_led_switch_device(&leds->ledbar_switch[3][0], 0x42, 15);
    led_controller_add_led_switch_device(&leds->ledbar_switch[3][1], 0x43, 0);
    led_controller_add_led_switch_device(&leds->ledbar_switch[3][2], 0x43, 1);
    led_controller_add_led_switch_device(&leds->ledbar_switch[3][3], 0x43, 2);
    led_controller_add_led_switch_device(&leds->ledbar_switch[3][4], 0x43, 3);
    /*20-24*/
    led_controller_add_led_switch_device(&leds->ledbar_switch[4][0], 0x43, 4);
    led_controller_add_led_switch_device(&leds->ledbar_switch[4][1], 0x43, 5);
    led_controller_add_led_switch_device(&leds->ledbar_switch[4][2], 0x43, 6);
    led_controller_add_led_switch_device(&leds->ledbar_switch[4][3], 0x43, 7);
    led_controller_add_led_switch_device(&leds->ledbar_switch[4][4], 0x43, 8);
}


void initVisuals() {
    init_servos();
    initializeLEDs(&leds);
    nixie_controller_init(nixie_set_number, nixie_power_off, millis);
    time_display_init(setDeviceChannelServoPulseDuration_wrapper, 0x40, 0);
    
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

servo_pos_t switches_pos_to_servo_pos(switch_pos_t pos) {
    switch (pos) {
    case high_switch:
        return servo_pos_high;
    case mid_switch:
        return servo_pos_center;
    case low_switch:
        return servo_pos_low;
    default:
        return servo_pos_undefined;
    }
}

void markActiveSegments(
    int Z[NO_OF_3_WAY_LINES][NO_OF_SWITCHES_PER_LINE + 1], const three_way_switches_array_t switches, int8_t startx) {
    Z[startx][0] = 1;
    int newk;
    int k = startx;
    for (int c = 1; c < NO_OF_SWITCHES_PER_LINE + 1; ++c) {
        newk = k;
        switch (switches[k][c - 1].position) {
        case mid_switch:
            {
                Z[k][c] = Z[k][c - 1];
                newk    = k;
                break;
            }
        case low_switch:
            if (k + 1 < 5) {
                Z[k + 1][c] = Z[k][c - 1];
                newk        = k + 1;
            }
            break;
        case high_switch:
            if (k - 1 >= 0) {
                Z[k - 1][c] = Z[k][c - 1];
                newk        = k - 1;
            }
            break;
        default:
            LOG_ERROR("No such switch");
        }
        k = newk;
    }
}

void drawLevel(map_t* map) {
    static long flash_on_timestamp;
    static int flash_flag;
    int activeSegments[5][6] = {0};
    int start_line           = -1;
    for (int i = 0; i < NO_OF_3_WAY_LINES; ++i) {
        if (map->start_nodes[i] != 0) {
            start_line = i;
        }
    }
    markActiveSegments(activeSegments, map->switches, start_line);
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
        leds.led_lamp[0][lineIndex].state = (map->start_nodes[lineIndex] != 0) ? lamp_state_on : lamp_state_off;
        leds.led_lamp[1][lineIndex].state = (activeSegments[lineIndex][5] != 0) ? lamp_state_on : lamp_state_off;
    }
    // at rotary encoder position make it blinking
    led_controller_update(&leds);
    servo_ctrl_update(servos);
}

void appendInfo(const int end_goal, const int time_left, const int current_level) {
    unsigned int max_time = switches_time_get_level_time(current_level);
    time_display_set_time((unsigned int) time_left, max_time);
    nixie_controller_diplay_number(end_goal);
    sound_module.goToTrack(current_level / 5);
}

void get_controls_status(rotary_enc_t* rotary) {
    rotary_t rotary_status;
    Rotary_get_status(&rotary_status);
    rotary->direction = rotary_status.direction;
    rotary->button    = rotary_status.button;
    if (rotary->button) {
        change_position_flag = true;
    }
    // if no button played for a long time stop playing sound ???
}

void shutDownDevice() {
    // YOLO (literaly): No shutdown procedure
}

void init_mui_structures(userInterface_t** gui) {
    pca9685.setDeviceChannelServoPulseDuration(0x40, 0, 16000);
    led_controller_test(&leds);
    nixie_controller_test();
    servo_ctrl_test(servos);
    *gui                        = (userInterface_t*) malloc(sizeof(userInterface_t));
    (*gui)->initVisuals         = initVisuals;
    (*gui)->initControls        = initRotaryEncoder;
    (*gui)->drawLevel           = drawLevel;
    (*gui)->appendInfo          = appendInfo;
    (*gui)->get_controls_status = get_controls_status;
    (*gui)->terminate           = shutDownDevice;
}

void delete_mui_structures(userInterface_t** gui) {
    free(*gui);
    *gui = NULL;
}
