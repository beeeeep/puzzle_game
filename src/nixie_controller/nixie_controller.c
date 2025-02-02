
#include "nixie_controller.h"

static nixie_driver_set_pins_t  nixie_driver_set_pins;
static nixie_driver_set_number_t nixie_driver_set_number;
static nixie_driver_power_off_t nixie_driver_power_off;
static milliseconds_t __nixie_milliseconds;

void nixie_controller_init(nixie_driver_set_pins_t nixie_driver_set_pins_p, nixie_driver_set_number_t nixie_driver_set_number_p,
    nixie_driver_power_off_t nixie_driver_power_off_p, milliseconds_t milliseconds_p) {
    nixie_driver_set_pins   = nixie_driver_set_pins_p;
    nixie_driver_power_off  = nixie_driver_power_off_p;
    nixie_driver_set_number = nixie_driver_set_number_p;
    __nixie_milliseconds    = milliseconds_p;

    nixie_driver_set_pins();
    nixie_controller_diplay_number(NIXIE_OFF);
}

void nixie_controller_diplay_number(unsigned char number) {
    static unsigned char number_prev;
    if (number_prev == number) {
        return;
    }
    if (number == NIXIE_OFF) {
        nixie_driver_power_off();
    } else if (number < 10) {
        nixie_driver_set_number(number);
    }
    number_prev = number;
}


void nixie_controller_flash_number(unsigned char number) {
    static unsigned long timestamp;

    unsigned char decades = number / 10;
    unsigned char monades = number % 10;
    unsigned char stage   = 0;

    nixie_controller_diplay_number(decades);
    timestamp = __nixie_milliseconds();
    while (__nixie_milliseconds() - timestamp < NUMBER_FLASH_ON_TIME)
        ; // Wait for some time
    nixie_controller_diplay_number(NIXIE_OFF);
    timestamp = __nixie_milliseconds();
    while (__nixie_milliseconds() - timestamp < NUMBER_FLASH_OFF_TIME)
        ; // Wait for some time
    nixie_controller_diplay_number(monades);
    timestamp = __nixie_milliseconds();
    while (__nixie_milliseconds() - timestamp < NUMBER_FLASH_ON_TIME)
        ; // Wait for some time
    nixie_controller_diplay_number(NIXIE_OFF);
    timestamp = __nixie_milliseconds();
    while (__nixie_milliseconds() - timestamp < NUMBER_FLASH_OFF_TIME);

}

void nixie_controller_test(void) {
    static unsigned long timestamp;
    for (unsigned char i = 0; i < 10; i++) {
        nixie_controller_diplay_number(i);
        timestamp = __nixie_milliseconds();
        while (__nixie_milliseconds() - timestamp < 1000)
            ; // Wait for some time
    }
}
