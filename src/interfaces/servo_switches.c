#include "servo_switches.h"

servo_pos_t switch_pos_to_servo_pos(const switch_pos_t* pos)
{
    switch (*pos)
    {
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

servo_motor_t three_way_switch_to_servo_motor(const three_way_switch_t* t_switch)
{
    servo_motor_t servo;
    servo.position = switch_pos_to_servo_pos(&t_switch->position);
    servo.position_prev = switch_pos_to_servo_pos(&t_switch->position);
    servo.high_pos_value = 0;
    servo.center_pos_value = 0;
    servo.low_pos_value = 0;
    servo.ready = 0;
    servo.channel = 0;
    servo.pc9685_id = 0;
    return servo;
}