#include "leds_init.h"


#define LED_BAR_STATIC_SER_RCLK 16
#define LED_BAR_STATIC_SER_SCLK 17
#define LED_BAR_STATIC_SER_OE   8
#define LED_BAR_STATIC_SER_0    7
#define LED_BAR_STATIC_SER_1    5
#define LED_BAR_STATIC_SER_2    18
#define LED_BAR_STATIC_SER_3    15

#define LED_LAMP_SCLK 47
#define LED_LAMP_RCLK 21
#define LED_LAMP_OE   48
#define LED_LAMP_SER  15

PCA9685 * pca9685;

const int led_lamp_offsets[16] = {64, 32, 16, 8, 4, 2, 1, 32768, 16384, 8192};
const int ledbar_offsets[4][8] = {{
                                64,
                                32,
                                16,
                                8,
                                4,
                                2,
                                1,
                                32768,
                            },
    {64, 32, 16, 8, 4, 2, 1, 32768}, {64, 32, 16, 8, 4, 2, 1, 32768}, {64, 32, 16, 8, 4, 2, 1, 32768}};

unsigned long millis_wrapper(void) {
    return (unsigned long) millis();
}

void led_bar_set_pwm_duty_cycle(unsigned char device_ID, unsigned char channel, unsigned int power) {
    pca9685->setDeviceChannelDutyCycle(device_ID, channel, power);
}

void led_bar_static_set_state(unsigned char device_no, unsigned char channel, unsigned char state) {
    unsigned char ser_pin = 0;
    static unsigned int led_bar_data_reg[4];
    switch (device_no) {
    case 0:
        ser_pin = LED_BAR_STATIC_SER_0;
        break;
    case 1:
        ser_pin = LED_BAR_STATIC_SER_1;
        break;
    case 2:
        ser_pin = LED_BAR_STATIC_SER_2;
        break;
    case 3:
        ser_pin = LED_BAR_STATIC_SER_3;
        break;
    }
    if (state == 1) {
        led_bar_data_reg[device_no] |= (1 << ledbar_offsets[device_no][channel]); // set to high
    } else {
        led_bar_data_reg[device_no] &= ~(1 << ledbar_offsets[device_no][channel]); // set to low
    }
    shiftOut(ser_pin, LED_LAMP_SCLK, MSBFIRST, 0xFF & (led_bar_data_reg[device_no] >> 8));
    shiftOut(ser_pin, LED_LAMP_SCLK, MSBFIRST, 0xFF & led_bar_data_reg[device_no]);
}

void led_lamp_set_state(unsigned char channel, unsigned char state) {
    static unsigned int led_lamp_data_reg;

    if (state == 1) {
        led_lamp_data_reg |= (1 << led_lamp_offsets[channel]); // set to high
    } else {
        led_lamp_data_reg &= ~(1 << led_lamp_offsets[channel]); // set to low
    }

    digitalWrite(LED_LAMP_RCLK, 0);
    shiftOut(LED_LAMP_SER, LED_LAMP_SCLK, MSBFIRST, 0xFF & (led_lamp_data_reg >> 8));
    shiftOut(LED_LAMP_SER, LED_LAMP_SCLK, MSBFIRST, 0xFF & led_lamp_data_reg);
    digitalWrite(LED_LAMP_SCLK, 1);
}

void initializeLEDs(leds_ctrl_str_t* leds)
{
    led_controller_init(millis_wrapper, led_bar_set_pwm_duty_cycle, led_bar_static_set_state,
    led_lamp_set_state, leds);
    digitalWrite(LED_LAMP_OE, 0);
    digitalWrite(LED_BAR_STATIC_SER_OE, 0);

    // Initialize the led bards that are on the switches, not placed in a for loop so they can be calibrated
    // idividually
    /*0-4*/
    led_controller_add_led_switch_device(&leds->ledbar_switch[0][0],0x42,0);
    led_controller_add_led_switch_device(&leds->ledbar_switch[0][1],0x42,1);
    led_controller_add_led_switch_device(&leds->ledbar_switch[0][2],0x42,2);
    led_controller_add_led_switch_device(&leds->ledbar_switch[0][3],0x42,3);
    led_controller_add_led_switch_device(&leds->ledbar_switch[0][4],0x42,4);
    /*5-9*/
    led_controller_add_led_switch_device(&leds->ledbar_switch[1][0],0x42,5);
    led_controller_add_led_switch_device(&leds->ledbar_switch[1][1],0x42,6);
    led_controller_add_led_switch_device(&leds->ledbar_switch[1][2],0x42,7);
    led_controller_add_led_switch_device(&leds->ledbar_switch[1][3],0x42,8);
    led_controller_add_led_switch_device(&leds->ledbar_switch[1][4],0x42,9);
    /*10-14*/
    led_controller_add_led_switch_device(&leds->ledbar_switch[2][0],0x42,10);
    led_controller_add_led_switch_device(&leds->ledbar_switch[2][1],0x42,11);
    led_controller_add_led_switch_device(&leds->ledbar_switch[2][2],0x42,12);
    led_controller_add_led_switch_device(&leds->ledbar_switch[2][3],0x42,13);
    led_controller_add_led_switch_device(&leds->ledbar_switch[2][4],0x42,14);
    /*15-19*/
    led_controller_add_led_switch_device(&leds->ledbar_switch[3][0],0x42,15);
    led_controller_add_led_switch_device(&leds->ledbar_switch[3][1],0x43,0);
    led_controller_add_led_switch_device(&leds->ledbar_switch[3][2],0x43,1);
    led_controller_add_led_switch_device(&leds->ledbar_switch[3][3],0x43,2);
    led_controller_add_led_switch_device(&leds->ledbar_switch[3][4],0x43,3);
    /*20-24*/
    led_controller_add_led_switch_device(&leds->ledbar_switch[4][0],0x43,4);
    led_controller_add_led_switch_device(&leds->ledbar_switch[4][1],0x43,5);
    led_controller_add_led_switch_device(&leds->ledbar_switch[4][2],0x43,6);
    led_controller_add_led_switch_device(&leds->ledbar_switch[4][3],0x43,7);
    led_controller_add_led_switch_device(&leds->ledbar_switch[4][4],0x43,8);
}

void leds_setPca9685(PCA9685* value)
{
    pca9685 = value;
}
