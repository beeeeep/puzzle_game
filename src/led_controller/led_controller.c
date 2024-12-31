
#include "led_controller.h"

static milliseconds_t ___millis;
static led_bar_set_pwm_duty_cycle_t led_bar_set_pwm_duty_cycle;
static led_bar_static_set_state_t led_bar_static_set_state;
static led_lamp_set_state_t led_lamp_set_state;

static void led_controller_set_led_switch_state(ledbar_switch_t* ledbar) {

    switch (ledbar->state) {
    case lamp_state_off:
        if (ledbar->state != ledbar->state_prev) {
            led_bar_set_pwm_duty_cycle(ledbar->pc9685_id, ledbar->channel, 0);
        }
        break;
    case lamp_state_blink:
        if (ledbar->blink_state == 1) {
            if (___millis() - ledbar->timestamp > BLINK_ON_TIME_MS) {
                ledbar->blink_power += BLINK_ON_INCREMENT;
                ledbar->timestamp = ___millis();
                led_bar_set_pwm_duty_cycle(ledbar->pc9685_id, ledbar->channel, ledbar->blink_power);
            }
            if (ledbar->blink_power == MAX_POWER) {
                ledbar->blink_state == 0;
            }
        } else {
            if (___millis() - ledbar->timestamp > BLINK_OFF_TIME_MS) {
                ledbar->blink_power -= BLINK_OFF_INCREMENT;
                led_bar_set_pwm_duty_cycle(ledbar->pc9685_id, ledbar->channel, ledbar->blink_power);
            }
            if (ledbar->blink_power == MIN_POWER) {
                ledbar->blink_state == 1;
            }
        }
        break;
    case lamp_state_on:
        if (ledbar->state != ledbar->state_prev) {
            led_bar_set_pwm_duty_cycle(ledbar->pc9685_id, ledbar->channel, MAX_POWER);
        }
        break;
    default:
        break;
    }
    ledbar->state_prev = ledbar->state;
}

static void led_controller_set_lamps_power(leds_ctrl_str_t* led_control_str) {
    if (led_control_str->led_lamp_power_left_col_prev != led_control_str->led_lamp_power_left_col) {
        led_bar_set_pwm_duty_cycle(led_control_str->led_lamp_left_col_pwr_pc9685_id,
            led_control_str->led_lamp_left_col_pwr_channel, led_control_str->led_lamp_power_left_col);
    }
    led_control_str->led_lamp_power_left_col_prev = led_control_str->led_lamp_power_left_col;

    if (led_control_str->led_lamp_power_right_col_prev != led_control_str->led_lamp_power_right_col) {
        led_bar_set_pwm_duty_cycle(led_control_str->led_lamp_right_col_pwr_pc9685_id,
            led_control_str->led_lamp_right_col_pwr_channel, led_control_str->led_lamp_power_right_col);
    }
    led_control_str->led_lamp_power_right_col_prev = led_control_str->led_lamp_power_right_col;
}

static void led_controller_set_ledbars_static_power(leds_ctrl_str_t* led_control_str) {
    if (led_control_str->ledbar_static_power_prev != led_control_str->ledbar_static_power) {
        led_bar_set_pwm_duty_cycle(led_control_str->ledbar_static_pwr_pc9685_id,
            led_control_str->ledbar_static_pwr_channel, led_control_str->ledbar_static_power);
    }
    led_control_str->ledbar_static_power_prev = led_control_str->ledbar_static_power;
}

static void led_controller_set_ledbar_static_state(ledbar_static_t* ledbar_static_str) {
    if (ledbar_static_str->state != ledbar_static_str->state_prev) {
        led_bar_static_set_state(ledbar_static_str->device_no, ledbar_static_str->channel, ledbar_static_str->state);
    }
    ledbar_static_str->state_prev = ledbar_static_str->state;
}

static void led_controller_set_lamp_state(led_lamp_t* led_lamp_str) {
    if (led_lamp_str->state != led_lamp_str->state_prev) {
        led_lamp_set_state(led_lamp_str->channel, led_lamp_str->state);
    }
    led_lamp_str->state_prev = led_lamp_str->state;
}


void led_controller_init(milliseconds_t millis_p, led_bar_set_pwm_duty_cycle_t led_bar_set_pwm_duty_cycle_p,
    led_bar_static_set_state_t led_bar_static_set_state_p, led_lamp_set_state_t led_lamp_set_state_p,
    leds_ctrl_str_t* leds) {
    unsigned char device_index  = 0;
    unsigned char channel_index = 0;

    // pass function pointers
    ___millis                     = millis_p;
    led_bar_set_pwm_duty_cycle = led_bar_set_pwm_duty_cycle_p;
    led_bar_static_set_state   = led_bar_static_set_state_p;
    led_lamp_set_state         = led_lamp_set_state_p;

    // Init static led bars
    for (int line = 0; line < 4; line++) {
        for (int col = NO_OF_STATIC_LEDBARS_LINES; col < NO_OF_STATIC_LEDBARS_PER_LINE; col++) {
            leds->ledbar_static[line][col].channel    = channel_index;
            leds->ledbar_static[line][col].device_no  = device_index;
            leds->ledbar_static[line][col].state      = lamp_state_off;
            leds->ledbar_static[line][col].state_prev = lamp_state_off;
            if (channel_index > 8) {
                channel_index = 0;
                device_index++;
            } else {
                channel_index++;
            }
        }
    }
    // Set init power to zero
  //  led_controller_set_lamps_power(0);
  //  led_controller_set_ledbars_static_power(0);
}

void led_controller_update(leds_ctrl_str_t* led_controller_str) {
    // Set VCC of lamps and static ledbars
   // led_controller_set_lamps_power(led_controller_str);
   // led_controller_set_ledbars_static_power(led_controller_str);

    // Set lamp states
    for (unsigned char line_index = 0; line_index < NO_OF_LED_LAMPS_PER_COL; line_index++) {
        for (unsigned char col_index = 0; col_index < NO_OF_LED_LAMPS_COL; col_index++) {
            led_controller_set_lamp_state(&led_controller_str->led_lamp[line_index][col_index]);
        }
    }
    // Set static ledbar states
    for (unsigned char line_index = 0; line_index < NO_OF_STATIC_LEDBARS_LINES; line_index++) {
        for (unsigned char col_index = 0; col_index < NO_OF_STATIC_LEDBARS_PER_LINE; col_index++) {
   //         led_controller_set_ledbar_static_state(&led_controller_str->ledbar_static[line_index][col_index]);
        }
    }
    // Set ledbars on servo switches states
    for (unsigned char line_index = 0; line_index < NO_OF_SWITCH_LEDS_LINES; line_index++) {
        for (unsigned char col_index = 0; col_index < NO_OF_SWITCH_LEDS_PER_LINE; col_index++) {
     //       led_controller_set_led_switch_state(&led_controller_str->ledbar_static[line_index][col_index]);
        }
    }
}

void led_controller_add_led_switch_device(
    ledbar_switch_t* ledbar_switch, unsigned int pc9685_id, unsigned char channel) {
    ledbar_switch->blink_power = 0;
    ledbar_switch->blink_state = 0;
    ledbar_switch->channel     = channel;
    ledbar_switch->pc9685_id   = pc9685_id;
    ledbar_switch->state       = 0;
    ledbar_switch->state_prev  = 0;
    ledbar_switch->timestamp   = 0;
}


void led_controller_test(leds_ctrl_str_t* led_controller_str) {

    static unsigned long timestamp;

//    for (unsigned char row = 0; row < NO_OF_STATIC_LEDBARS_LINES; row++) {
//        for (unsigned char col = 0; col < NO_OF_STATIC_LEDBARS_PER_LINE; col++) {
//            timestamp=___millis();
//            led_controller_str->ledbar_static[row][col].state = 1;
//            led_controller_set_ledbar_static_state(&led_controller_str->ledbar_static[row][col]);
//            while(___millis()-timestamp<LED_TEST_PERIOD);
//            led_controller_str->ledbar_static[row][col].state = 0;
//            led_controller_set_ledbar_static_state(&led_controller_str->ledbar_static[row][col]);
//            led_controller_update(led_controller_str);
//        }
//        
//    }
//
//    for (unsigned char row = 0; row < NO_OF_SWITCH_LEDS_LINES; row++) {
//        for (unsigned char col = 0; col < NO_OF_SWITCH_LEDS_PER_LINE; col++) {
//            timestamp=___millis();
//            led_controller_str->ledbar_switch[row][col].state = 1;
//            led_controller_set_led_switch_state(&led_controller_str->ledbar_switch[row][col]);
//            while(___millis()-timestamp<LED_TEST_PERIOD);
//            led_controller_str->ledbar_switch[row][col].state = 0;
//            led_controller_set_led_switch_state(&led_controller_str->ledbar_switch[row][col]);
//            led_controller_update(led_controller_str);
//        }
//    }

    for (unsigned char col = 0; col < NO_OF_LED_LAMPS_COL; col++) {
        for (unsigned char row = 0; row < NO_OF_LED_LAMPS_PER_COL; row++) {
            timestamp=___millis();
            led_controller_str->led_lamp[col][row].state = 1;
            led_controller_set_lamp_state(&led_controller_str->led_lamp[col][row]);
            led_controller_update(led_controller_str);
            while(___millis()-timestamp<LED_TEST_PERIOD);
            led_controller_str->led_lamp[col][row].state = 0;
            led_controller_set_lamp_state(&led_controller_str->led_lamp[col][row]);
            led_controller_update(led_controller_str);
        }
    }

}
