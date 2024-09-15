#ifndef INTERFACES_SERVO_SWITCHES_H
#define INTERFACES_SERVO_SWITCHES_H

#include "switches.h"
#include "servo_controller.h"

servo_pos_t switch_pos_to_servo_pos(const switch_pos_t* pos);

servo_motor_t three_way_switch_to_servo_motor(const three_way_switch_t* t_switch);

#endif // INTERFACES_SERVO_SWITCHES_H
