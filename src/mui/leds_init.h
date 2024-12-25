#ifndef LEDS_INIT_H
#define LEDS_INIT_H

#include "led_controller/led_controller.h"
#include "PCA9685.h"

void initializeLEDs(leds_ctrl_str_t* leds);
void leds_setPca9685(PCA9685* value);

#endif // LEDS_INIT_H