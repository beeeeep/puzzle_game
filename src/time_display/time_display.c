#include "time_display.h"

static time_display_set_pwm_duty_cycle_t time_display_set_pwm_duty_cycle;
static int device_ID_val;
static unsigned char channel_val;

void time_display_init(time_display_set_pwm_duty_cycle_t time_display_set_pwm_duty_cycle_p, unsigned char device_ID,
    unsigned char channel) {
    time_display_set_pwm_duty_cycle = time_display_set_pwm_duty_cycle_p;
    device_ID_val=device_ID;
    channel_val=channel;
    time_display_set_pwm_duty_cycle(device_ID, channel, MAX_PWM_SETTING);
}

void time_display_set_time(unsigned int current_time, unsigned int max_time) 
{
    float percentage_of_max_time=(float)(current_time/((float)max_time));
    static unsigned int current_time_pwm_value_prev;
    unsigned int current_time_pwm_value=(unsigned int)(MAX_PWM_SETTING*percentage_of_max_time);

    if(current_time_pwm_value!=current_time_pwm_value_prev)
    {
        time_display_set_pwm_duty_cycle(device_ID_val,channel_val,current_time_pwm_value);
    }
    current_time_pwm_value_prev=current_time_pwm_value;
}
