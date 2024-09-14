#ifndef LED_LINES_H
#define LED_LINES_H

#include <stdio.h>
#ifdef __cplusplus
 extern "C" {
#endif

#define NO_OF_PWM_LEDS 25

#define NO_OF_STATIC_LEDS 30


typedef enum lamp_state
{
    lamp_state_off,
    lamp_state_blink,
    lamp_state_on
}lamp_state_t;


typedef struct leds_bars
{
    unsigned char static_led[NO_OF_STATIC_LEDS];
    lamp_state_t  dynamic_led[NO_OF_PWM_LEDS];
}leds_bars_t;


typedef void (*led_bar_set_pwm_t) (unsigned char cntl_addr, unsigned int lamp_channel, unsigned int level);
typedef void (*led_bar_set_on_off_State) (unsigned char led_bar_index, unsigned char state);

void led_lines_init(void);

#ifdef __cplusplus
 }
#endif

#endif 