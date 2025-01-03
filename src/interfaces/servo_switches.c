#include "servo_switches.h"

servo_pos_t switch_pos_to_servo_pos(const switch_pos_t pos)
{
    switch (pos)
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
