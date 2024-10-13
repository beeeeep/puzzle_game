
#include "led_controller.h"

static milliseconds_t millis;
static led_bar_set_pwm_duty_cycle_t led_bar_set_pwm_duty_cycle;
static led_bar_static_set_state_t led_bar_static_set_state;
static led_lamp_set_state_t led_lamp_set_state;

static void led_controller_set_led_switch_state(ledbar_switch_t* ledbar, unsigned char state) {

    switch (state) {
    case lamp_state_off:
        if (state != ledbar->state_prev) {
            led_bar_set_pwm_duty_cycle(ledbar->pc9685_id, ledbar->channel, 0);
        }
        break;
    case lamp_state_blink:
        if (ledbar->blink_state == 1) {
            if (millis() - ledbar->timestamp > BLINK_ON_TIME_MS) {
                ledbar->blink_power += BLINK_ON_INCREMENT;
                ledbar->timestamp = millis();
                led_bar_set_pwm_duty_cycle(ledbar->pc9685_id, ledbar->channel, ledbar->blink_power);
            }
            if (ledbar->blink_power == MAX_POWER) {
                ledbar->blink_state == 0;
            }
        } else {
            if (millis() - ledbar->timestamp > BLINK_OFF_TIME_MS) {
                ledbar->blink_power -= BLINK_OFF_INCREMENT;
                led_bar_set_pwm_duty_cycle(ledbar->pc9685_id, ledbar->channel, ledbar->blink_power);
            }
            if (ledbar->blink_power == MIN_POWER) {
                ledbar->blink_state == 1;
            }
        }
        break;
    case lamp_state_on:
        if (state != ledbar->state_prev) {
            led_bar_set_pwm_duty_cycle(ledbar->pc9685_id, ledbar->channel, MAX_POWER);
        }
        break;
    default:
        break;
    }
    ledbar->state_prev = state;
}

static void led_controller_set_lamps_power(leds_ctrl_str_t *led_control_str) 
{
 if(led_control_str->led_lamp_power_prev!=led_control_str->led_lamp_power)   
 {
    led_bar_set_pwm_duty_cycle(led_control_str->led_lamp_pwr_pc9685_id,led_control_str->led_lamp_pwr_channel,led_control_str->led_lamp_power);
 }
 led_control_str->led_lamp_power_prev=led_control_str->led_lamp_power;
}

static void led_controller_set_ledbar_static_power(leds_ctrl_str_t *led_control_str) 
{
 if(led_control_str->ledbar_static_power_prev!=led_control_str->ledbar_static_power)   
 {
    led_bar_set_pwm_duty_cycle(led_control_str->ledbar_static_pwr_pc9685_id,led_control_str->ledbar_static_pwr_channel,led_control_str->ledbar_static_power);
 }
 led_control_str->ledbar_static_power_prev=led_control_str->ledbar_static_power;
}

static void led_controller_set_ledbar_static_state(ledbar_static_t *ledbar_static_str)
{
    if(ledbar_static_str->state!=ledbar_static_str->state_prev)
    {
        led_bar_static_set_state(ledbar_static_str->device_no,ledbar_static_str->channel,ledbar_static_str->state);
    }
    ledbar_static_str->state_prev=ledbar_static_str->state;
}

static void led_controller_set_lamp_state(led_lamp_t *led_lamp_str)
{
    if(led_lamp_str->state!=led_lamp_str->state_prev)
    {
        led_bar_static_set_state(led_lamp_str->device_no,led_lamp_str->channel,led_lamp_str->state);
    }
    led_lamp_str->state_prev=led_lamp_str->state;
}


void led_controller_init(milliseconds_t millis_p, led_bar_set_pwm_duty_cycle_t led_bar_set_pwm_duty_cycle_p,
    led_bar_static_set_state_t led_bar_static_set_state_p, led_lamp_set_state_t led_lamp_set_state_p) {
    millis                     = millis_p;
    led_bar_set_pwm_duty_cycle = led_bar_set_pwm_duty_cycle_p;
    led_bar_static_set_state   = led_bar_static_set_state_p;
    led_lamp_set_state         = led_lamp_set_state_p;
}

void led_controller_update(leds_ctrl_str_t *led_controller_str) 
{
    //Set VCC of lamps and static ledbars
    led_controller_set_lamps_power(led_controller_str); 
    led_controller_set_ledbar_static_power(led_controller_str); 

    //Set lamp states
    for(unsigned char line_index=0; line_index<NO_OF_LED_LAMPS_PER_COL; line_index++)
    {
        for(unsigned char col_index=0; col_index<NO_OF_LED_LAMPS_COL; col_index++)
        {
            led_controller_set_lamp_state(&led_controller_str->led_lamp[line_index][col_index]);
        }
    }
    //Set static ledbar states
    for(unsigned char line_index=0; line_index<NO_OF_STATIC_LEDBARS_LINES; line_index++)
    {
        for(unsigned char col_index=0; col_index<NO_OF_STATIC_LEDBARS_PER_LINE; col_index++)
        {
            led_controller_set_ledbar_static_state(&led_controller_str->ledbar_static[line_index][col_index]);
        }
    }
    //Set ledbars on servo switches states
    for(unsigned char line_index=0; line_index<NO_OF_SWITCH_LEDS_LINES; line_index++)
    {
        for(unsigned char col_index=0; col_index<NO_OF_SWITCH_LEDS_PER_LINE; col_index++)
        {
            led_controller_set_ledbar_static_state(&led_controller_str->ledbar_static[line_index][col_index]);
        }
    }
}


