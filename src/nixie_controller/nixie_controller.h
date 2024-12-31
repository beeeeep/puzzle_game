#ifndef NIXIE_CONTROLLER_H
#define NIXIE_CONTOROLLER_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#define NIXIE_OFF 255
#define NUMBER_FLASH_ON_TIME 2000
#define NUMBER_FLASH_OFF_TIME 500

typedef void (*nixie_driver_set_pins_t) (void);
typedef void (*nixie_driver_set_number_t) (unsigned char number);
typedef void (*nixie_driver_power_off_t) (void);
typedef unsigned long (*milliseconds_t) (void);


void nixie_controller_init(nixie_driver_set_pins_t nixie_driver_set_pins_p, nixie_driver_set_number_t nixie_driver_set_number_p,
    nixie_driver_power_off_t nixie_driver_power_off_p, milliseconds_t milliseconds_p);

void nixie_controller_diplay_number(unsigned char number);

//Displays 2 digit numbers for a short time, THIS IS A BLOCKING FUNCTION
void nixie_controller_flash_number(unsigned char number);

void nixie_controller_test(void);

#ifdef __cplusplus
}
#endif
#endif //NIXIE_CONTROLLER_H