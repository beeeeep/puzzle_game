#ifndef SERVO_CONTROLLER_H
#define SERVO_CONTROLLER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#define NO_OF_SERVOS 16
#define SERVO_UPDATE_RATE_MS 500

typedef enum
{
    servo_pos_high,
    servo_pos_center,
    servo_pos_low,
    servo_pos_undefined
} servo_pos_t;


typedef void (*servo_ctrl_set_pwm_t) (unsigned char cntl_addr, unsigned int servo_channel, unsigned int pos);

typedef unsigned long (*milliseconds_t) (void);

typedef struct servo_motor
{
    servo_pos_t position;
    servo_pos_t position_prev;
    unsigned int high_pos_value;
    unsigned int center_pos_value;
    unsigned int low_pos_value;
    unsigned char ready;
    unsigned char channel;
    unsigned int pc9685_id;
} servo_motor_t;


void servo_ctrl_init(milliseconds_t millis_func_p,servo_ctrl_set_pwm_t servo_ctrl_set_pwm_func_p);

void servo_ctrl_add_device(servo_motor_t servos[NO_OF_SERVOS], unsigned char device_index, unsigned int pc9685_id, unsigned char channel, unsigned int high_pos_value, unsigned int center_pos_value,  unsigned int low_pos_value );

void servo_ctrl_change_position(servo_motor_t *servo);

void servo_ctrl_update(servo_motor_t servos[NO_OF_SERVOS]);

#ifdef __cplusplus
}
#endif

#endif // SERVO_CONTROLLER_H