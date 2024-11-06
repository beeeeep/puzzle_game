#ifndef TIME_DISPLAY_H
#define TIME_DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>


#define MAX_PWM_SETTING 16000

typedef void (*time_display_set_pwm_duty_cycle_t) (unsigned char device_ID, unsigned char channel, unsigned int power);

void time_display_init(time_display_set_pwm_duty_cycle_t time_display_set_pwm_duty_cycle_p, unsigned char device_ID, unsigned char channel);

void time_display_set_time(unsigned int current_time, unsigned int max_time);


#ifdef __cplusplus
}
#endif
#endif //TIME_DISPLAY_H