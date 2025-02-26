#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#define NO_OF_SWITCH_LEDS_PER_LINE 5
#define NO_OF_SWITCH_LEDS_LINES 5

#define NO_OF_STATIC_LEDBARS_PER_LINE 6
#define NO_OF_STATIC_LEDBARS_LINES 5

#define NO_OF_LED_LAMPS_PER_COL 5
#define NO_OF_LED_LAMPS_COL 2


#define BLINK_NO_OF_STEPS 50
#define BLINK_ON_TIME_MS 300
#define BLINK_OFF_TIME_MS 200

#define BLINK_POWERING_UP_TOTAL_TIME_MS 200
#define BLINK_POWERING_DOWN_TOTAL_TIME_MS 200

#define BLINK_ON_INCREMENT_TIME_MS (BLINK_POWERING_UP_TOTAL_TIME_MS/BLINK_NO_OF_STEPS)
#define BLINK_OFF_INCREMENT_TIME_MS (BLINK_POWERING_DOWN_TOTAL_TIME_MS/BLINK_NO_OF_STEPS)

#define BLINK_ON_INCREMENT ((MAX_BLINK_POWER-MIN_POWER)/BLINK_NO_OF_STEPS)
#define BLINK_OFF_INCREMENT ((MAX_BLINK_POWER-MIN_POWER)/BLINK_NO_OF_STEPS)


#define MIN_POWER 0
#define MAX_POWER 100  
#define MAX_BLINK_POWER 100

#define LED_TEST_PERIOD 200

typedef enum lamp_state
{
    lamp_state_off,
    lamp_state_on,
    lamp_state_blink,
}lamp_state_t;

typedef enum blink_state{
blink_off,
blink_powering_up,
blink_on,
blink_powering_down
}blink_state_t;

typedef struct ledbar_switch
{
    lamp_state_t state;
    lamp_state_t state_prev;
    unsigned int pc9685_id;
    unsigned char channel;
    unsigned long timestamp;
    blink_state_t blink_state;
     int blink_power;
}ledbar_switch_t;

typedef struct ledbar_static
{
    lamp_state_t state;
    lamp_state_t state_prev;
    unsigned int  device_no;
    unsigned char channel;
}ledbar_static_t;

typedef struct led_lamp
{
    lamp_state_t state;
    lamp_state_t state_prev;
    unsigned char channel;
}led_lamp_t;

typedef struct leds_ctrl_str 
{
    ledbar_static_t ledbar_static[NO_OF_STATIC_LEDBARS_LINES][NO_OF_STATIC_LEDBARS_PER_LINE];
    ledbar_switch_t  ledbar_switch[NO_OF_SWITCH_LEDS_PER_LINE][NO_OF_SWITCH_LEDS_LINES];
    led_lamp_t led_lamp[NO_OF_LED_LAMPS_PER_COL][NO_OF_LED_LAMPS_COL];
    
    unsigned int led_lamp_left_col_pwr_pc9685_id;
    unsigned char led_lamp_left_col_pwr_channel;
    unsigned int led_lamp_right_col_pwr_pc9685_id;
    unsigned char led_lamp_right_col_pwr_channel;
    unsigned int led_lamp_power_left_col;
    unsigned int led_lamp_power_left_col_prev;
    unsigned int led_lamp_power_right_col;
    unsigned int led_lamp_power_right_col_prev;
    
    unsigned int ledbar_static_pwr_pc9685_id;
    unsigned char ledbar_static_pwr_channel;
    unsigned int ledbar_static_power;
    unsigned int ledbar_static_power_prev;

}leds_ctrl_str_t;

typedef void (*led_bar_set_pwm_duty_cycle_t) (unsigned char device_ID, unsigned char channel, unsigned int power);
typedef void (*led_bar_static_set_state_t) (unsigned char device_no, unsigned char channel, unsigned char state);
typedef void (*led_lamp_set_state_t) (unsigned char channel, unsigned char state);
typedef unsigned long (*milliseconds_t) (void);

void led_controller_init(milliseconds_t millis_p, led_bar_set_pwm_duty_cycle_t led_bar_set_pwm_duty_cycle_p,
    led_bar_static_set_state_t led_bar_static_set_state_p, led_lamp_set_state_t led_lamp_set_state_p, leds_ctrl_str_t *leds);

void led_controller_update(leds_ctrl_str_t *led_controller_str); 

void led_controller_add_led_switch_device(ledbar_switch_t* ledbar_switch,unsigned int pc9685_id ,unsigned char channel);

void led_controller_test(leds_ctrl_str_t* led_controller_str);

void led_controller_victory_round(leds_ctrl_str_t* led_controller_str) ;

#ifdef __cplusplus
}
#endif

#endif 