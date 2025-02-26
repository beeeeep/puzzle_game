#ifndef INTERFACES_SERVO_SWITCHES_H
#define INTERFACES_SERVO_SWITCHES_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CMAKE_BUILD
#include "switches.h"
#include "servo_controller.h"
#else
#include "../switches/switches.h"
#include "../servo_controller/servo_controller.h"
#endif

servo_pos_t switch_pos_to_servo_pos(const switch_pos_t pos);


#ifdef __cplusplus
}
#endif

#endif // INTERFACES_SERVO_SWITCHES_H
