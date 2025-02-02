#include "mui.h"

#include "src/led_controller/led_controller.h"
#include "src/misc.h"
#include "src/nixie_controller/nixie_controller.h"
#include "src/rotary/rotary.h"
#include "src/servo_controller/servo_controller.h"
#include "src/switches/switches.h"
#include "src/time_display/time_display.h"
#include "src/interfaces/servo_switches.h"
#include "src/sound/sound_module2.h"
#include <stdlib.h>

#define ROTARY_A_PIN      42
#define ROTARY_B_PIN      2
#define ROTARY_BUTTON_PIN 1

#define SCL_PIN 40
#define SDA_PIN 41

#define PCA9685_OUTPUT_ENABLE_PIN

#define NIXIE_CLOCK_PIN     12
#define NIXIE_SER_PIN       10
#define NIXIE_LATCH_PIN     9
#define NIXIE_OUTPUT_ENABLE 11

#define LED_BAR_STATIC_SER_RCLK 16
#define LED_BAR_STATIC_SER_SCLK 17
#define LED_BAR_STATIC_SER_OE   8
#define LED_BAR_STATIC_SER_0    7
#define LED_BAR_STATIC_SER_1    5
#define LED_BAR_STATIC_SER_2    18
#define LED_BAR_STATIC_SER_3    15

#define LED_LAMP_SCLK  21
#define LED_LAMP_LATCH 47
#define LED_LAMP_OE    48
#define LED_LAMP_SER   45


#define SOUND_MODULE_PIN_S1 36
#define SOUND_MODULE_PIN_S2 35
#define SOUND_MODULE_PIN_S3 0
#define SOUND_MODULE_PIN_S6 37

static PCA9685 pca9685;
static servo_motor_t servos[NO_OF_SERVOS];
static leds_ctrl_str_t leds;
static rotary_t rotary;
static GPD2846 sound_module(SOUND_MODULE_PIN_S1, SOUND_MODULE_PIN_S2, SOUND_MODULE_PIN_S3, SOUND_MODULE_PIN_S6, 20);

static bool change_position_flag = false;
static control_index_t control_position;


static int nixie_data[16] = {
    /*0*/ 4,
    /*1*/ 2,
    /*2*/ 1,
    /*3*/ 32768,
    /*4*/ 16384,
    /*5*/ 8192,
    /*6*/ 64,
    /*7*/ 32,
    /*8*/ 16,
    /*9*/ 8};

static int test_a_read, test_b_read, test_button_read, test_a_value, test_b_value;


static const int led_lamp_offsets[15] = {7, 6, 5, 4, 3, 2, 1, 0, 8, 15, 14, 13, 12, 11, 10};

static const int ledbar_offsets[4][8] = {{
                                             7,
                                             6,
                                             5,
                                             4,
                                             3,
                                             2,
                                             1,
                                             0,
                                         },
    {
        7,
        6,
        5,
        4,
        3,
        2,
        1,
        0,
    },
    {
        7,
        6,
        5,
        4,
        3,
        2,
        1,
        0,
    },
    {
        7,
        6,
        5,
        4,
        3,
        2,
        1,
        0,
    }};

// WRAPPER FUNCTIONS

unsigned int random_number_seed(void)
{
    return (unsigned int) analogRead(6);
}


void nixie_init_pins(void) {
    pinMode(NIXIE_CLOCK_PIN, OUTPUT);
    pinMode(NIXIE_SER_PIN, OUTPUT);
    pinMode(NIXIE_LATCH_PIN, OUTPUT);
    pinMode(NIXIE_OUTPUT_ENABLE, OUTPUT);
    digitalWrite(NIXIE_OUTPUT_ENABLE, LOW);
}

void nixie_set_number(unsigned char number) {
    digitalWrite(NIXIE_OUTPUT_ENABLE, LOW);
    digitalWrite(NIXIE_LATCH_PIN, LOW);
    shiftOut(NIXIE_SER_PIN, NIXIE_CLOCK_PIN, MSBFIRST, 0xFF & (nixie_data[number] >> 8));
    shiftOut(NIXIE_SER_PIN, NIXIE_CLOCK_PIN, MSBFIRST, 0xFF & nixie_data[number]);
    digitalWrite(NIXIE_LATCH_PIN, HIGH);
}

void nixie_power_off(void) {
    digitalWrite(NIXIE_OUTPUT_ENABLE, HIGH);
    digitalWrite(NIXIE_LATCH_PIN, LOW);
    shiftOut(NIXIE_SER_PIN, NIXIE_CLOCK_PIN, MSBFIRST, 0xFF);
    shiftOut(NIXIE_SER_PIN, NIXIE_CLOCK_PIN, MSBFIRST, 0xFF);
    digitalWrite(NIXIE_LATCH_PIN, HIGH);
}

void setDeviceChannelServoPulseDuration_wrapper(
    unsigned char cntl_addr, unsigned char servo_channel, unsigned int pos) {
    pca9685.setDeviceChannelServoPulseDuration(cntl_addr, servo_channel, pos);
}

unsigned long millis_wrapper(void) {
    return (unsigned long) millis();
}

unsigned char rotary_read_pin_A(void) {
    test_a_read  = 1;
    test_a_value = digitalRead(ROTARY_A_PIN);
    return (unsigned char) digitalRead(ROTARY_A_PIN);
}

unsigned char rotary_read_pin_B(void) {
    test_b_read  = 1;
    test_b_value = digitalRead(ROTARY_B_PIN);
    return (unsigned char) digitalRead(ROTARY_B_PIN);
}

unsigned char rotary_read_button(void) {
    test_button_read = 1;

    return (unsigned char) digitalRead(ROTARY_BUTTON_PIN);
}

void initRotaryEncoder() {
    pinMode(ROTARY_A_PIN, INPUT_PULLUP);
    pinMode(ROTARY_B_PIN, INPUT_PULLUP);
    pinMode(ROTARY_BUTTON_PIN, INPUT_PULLUP);
    attachInterrupt(ROTARY_A_PIN, Rotary_AB_pin_callback_fuction, CHANGE);
    attachInterrupt(ROTARY_B_PIN, Rotary_AB_pin_callback_fuction, CHANGE);
    attachInterrupt(ROTARY_BUTTON_PIN, Rotary_button_pin_callback_fuction, CHANGE);
    Rotary_init(rotary_read_pin_A, rotary_read_pin_B, rotary_read_button, millis_wrapper);
}


void led_bar_set_pwm_duty_cycle(unsigned char device_ID, unsigned char channel, unsigned int power) {
    pca9685.setDeviceChannelDutyCycle(device_ID, channel, power);
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


    digitalWrite(LED_BAR_STATIC_SER_OE, LOW);
    digitalWrite(LED_BAR_STATIC_SER_RCLK, LOW);

    for (int i = 0; i < 8; i++)  {
        digitalWrite(LED_BAR_STATIC_SER_0, !!(led_bar_data_reg[0] & (1 << (7 - i))));
        digitalWrite(LED_BAR_STATIC_SER_1, !!(led_bar_data_reg[1] & (1 << (7 - i))));
        digitalWrite(LED_BAR_STATIC_SER_2, !!(led_bar_data_reg[2] & (1 << (7 - i))));
        digitalWrite(LED_BAR_STATIC_SER_3, !!(led_bar_data_reg[3] & (1 << (7 - i))));    
        digitalWrite(LED_BAR_STATIC_SER_SCLK, HIGH);
        digitalWrite(LED_BAR_STATIC_SER_SCLK, LOW);        
    }
    

    //shiftOut(ser_pin, LED_BAR_STATIC_SER_SCLK, MSBFIRST, 0xFF & (led_bar_data_reg[device_no] >> 8));
   // shiftOut(ser_pin, LED_BAR_STATIC_SER_SCLK, MSBFIRST, 0xFF & led_bar_data_reg[device_no]);


    digitalWrite(LED_BAR_STATIC_SER_RCLK, HIGH);
}

void led_lamp_set_state(unsigned char channel, unsigned char state) {
    static unsigned int led_lamp_data_reg;

    if (state == 1) {
        led_lamp_data_reg |= (1 << led_lamp_offsets[channel]); // set to high
    } else {
        led_lamp_data_reg &= ~(1 << led_lamp_offsets[channel]); // set to low
    }
    digitalWrite(LED_LAMP_OE, LOW);
    digitalWrite(LED_LAMP_LATCH, LOW);
    shiftOut(LED_LAMP_SER, LED_LAMP_SCLK, MSBFIRST, 0xFF & (led_lamp_data_reg >> 8));
    shiftOut(LED_LAMP_SER, LED_LAMP_SCLK, MSBFIRST, 0xFF & led_lamp_data_reg);
    digitalWrite(LED_LAMP_LATCH, HIGH);
}

// Normal functions
void test_peripherals();
void init_servos();
void initializeLEDs(leds_ctrl_str_t* leds);
void initializePca9685();
servo_pos_t switches_pos_to_servo_pos(switch_pos_t pos);
void markActiveSegments(
    int Z[NO_OF_3_WAY_LINES][NO_OF_SWITCHES_PER_LINE + 1], const three_way_switches_array_t switches, int8_t startx);


void initVisuals() {
    static bool needInitialization = true;
    if (needInitialization) {
        initializePca9685(); // This needs to be here because the pca9685 is used in the initialization of everything
                             // else
        initializeLEDs(&leds);                     
        init_servos();
        time_display_init(setDeviceChannelServoPulseDuration_wrapper, 0x40, 11);
       
        nixie_controller_init(nixie_init_pins, nixie_set_number, nixie_power_off, millis);
        needInitialization = false;
       test_peripherals();
        LOG_INFO("Visuals initialized");
    }
}


void init_servos() {
    // Init Servos, these are not placed in a for loop so its easier to calibrate then idividually
    /*0-4*/
    servo_ctrl_init(millis_wrapper, setDeviceChannelServoPulseDuration_wrapper);
    servo_ctrl_add_device(servos, 0, 0x42, 3, 1900, 1400, 1000); // checked
    servo_ctrl_add_device(servos, 5, 0x42, 0, 1900, 1500, 1000); // checked
    servo_ctrl_add_device(servos, 10, 0x42, 2, 1900, 1450, 1000); // checked
    servo_ctrl_add_device(servos, 15, 0x42, 4, 2100, 1650, 1100); // checked
    servo_ctrl_add_device(servos, 20, 0x42, 1, 1900, 1500, 1000); // checked
    /*5-9*/
    servo_ctrl_add_device(servos, 1, 0x42, 6, 2000, 1500, 1000);
    servo_ctrl_add_device(servos, 6, 0x42, 8, 2000, 1500, 1000);
    servo_ctrl_add_device(servos, 11, 0x42, 9, 2000, 1550, 1000);
    servo_ctrl_add_device(servos, 16, 0x42, 5, 1900, 1400, 1000); // checked
    servo_ctrl_add_device(servos, 21, 0x42, 7, 1300, 700, 500);
    /*10-14*/
    servo_ctrl_add_device(servos, 2, 0x42, 10, 1900, 1500, 1000);
    servo_ctrl_add_device(servos, 7, 0x42, 12, 2100, 1700, 1200);
    servo_ctrl_add_device(servos, 12, 0x42, 14, 1900, 1500, 1000);
    servo_ctrl_add_device(servos, 17, 0x42, 11, 2200, 1800, 1300);
    servo_ctrl_add_device(servos, 22, 0x42, 13, 2450, 2100, 1600);

    /*15-19*/
    servo_ctrl_add_device(servos, 3, 0x40, 1, 2000, 1500, 1000);
    servo_ctrl_add_device(servos, 8, 0x40, 4, 2000, 1500, 1000);
    servo_ctrl_add_device(servos, 13, 0x40, 0, 2000, 1550, 1000);
    servo_ctrl_add_device(servos, 18, 0x40, 2, 2000, 1500, 1000);
    servo_ctrl_add_device(servos, 23, 0x40, 3, 2300, 1900, 1400);
    /*20-24*/
    servo_ctrl_add_device(servos, 4, 0x40, 5, 1950, 1450, 950);
    servo_ctrl_add_device(servos, 9, 0x40, 8, 1900, 1400, 900);
    servo_ctrl_add_device(servos, 14, 0x40, 7, 2150, 1670, 1150);
    servo_ctrl_add_device(servos, 19, 0x40, 6, 2050, 1600, 1050);
    servo_ctrl_add_device(servos, 24, 0x40, 9, 2050, 1650, 1150);


    for (int i = 0; i < NO_OF_SERVOS; i++) {
        servos[i].position = servo_pos_center;
    }
    LOG_INFO("All servos devices are added");
    int counter = 0;
    do {
        counter = servo_ctrl_update(servos);
    } while (counter > 0);

    LOG_INFO("Servos initialized");
}

void initializeLEDs(leds_ctrl_str_t* leds) {

    pinMode(LED_LAMP_SCLK, OUTPUT);
    pinMode(LED_LAMP_LATCH, OUTPUT);
    pinMode(LED_LAMP_OE, OUTPUT);
    pinMode(LED_LAMP_SER, OUTPUT);

    pinMode(LED_BAR_STATIC_SER_RCLK, OUTPUT);
    pinMode(LED_BAR_STATIC_SER_SCLK, OUTPUT);
    pinMode(LED_BAR_STATIC_SER_OE, OUTPUT);
    pinMode(LED_BAR_STATIC_SER_0, OUTPUT);
    pinMode(LED_BAR_STATIC_SER_1, OUTPUT);
    pinMode(LED_BAR_STATIC_SER_2, OUTPUT);
    pinMode(LED_BAR_STATIC_SER_3, OUTPUT);
    digitalWrite(LED_BAR_STATIC_SER_OE, HIGH);
    digitalWrite(LED_LAMP_OE, HIGH);

    // leds->led_lamp_left_col_pwr_pc9685_id=...
    // leds->led_lamp_right_col_pwr_pc9685_id=...
    leds->ledbar_static_pwr_pc9685_id = 0x40;
    leds->ledbar_static_pwr_channel   = 12;

    // Configure the Lamps

    leds->led_lamp[0][0].channel = 0;
    leds->led_lamp[1][0].channel = 1;
    leds->led_lamp[2][0].channel = 2;
    leds->led_lamp[3][0].channel = 3;
    leds->led_lamp[4][0].channel = 4;
    leds->led_lamp[0][1].channel = 5;
    leds->led_lamp[1][1].channel = 6;
    leds->led_lamp[2][1].channel = 7;
    leds->led_lamp[3][1].channel = 8;
    leds->led_lamp[4][1].channel = 9;


    leds->ledbar_static[0][0].device_no = 2; //?
    leds->ledbar_static[0][0].channel   = 0; //?
    leds->ledbar_static[1][0].device_no = 2; //?
    leds->ledbar_static[1][0].channel   = 1; //?
    leds->ledbar_static[2][0].device_no = 2; //>
    leds->ledbar_static[2][0].channel   = 2; //?
    leds->ledbar_static[3][0].device_no = 2; //
    leds->ledbar_static[3][0].channel   = 3; //
    leds->ledbar_static[4][0].device_no = 2; ///
    leds->ledbar_static[4][0].channel   = 4; //

    leds->ledbar_static[0][1].device_no = 2; //
    leds->ledbar_static[0][1].channel   = 5; //
    leds->ledbar_static[1][1].device_no = 2; //
    leds->ledbar_static[1][1].channel   = 6; //
    leds->ledbar_static[2][1].device_no = 2; //
    leds->ledbar_static[2][1].channel   = 7; //
    leds->ledbar_static[3][1].device_no = 0; //
    leds->ledbar_static[3][1].channel   = 0; //
    leds->ledbar_static[4][1].device_no = 0; //
    leds->ledbar_static[4][1].channel   = 1; //


    leds->ledbar_static[0][2].device_no = 0; //
    leds->ledbar_static[0][2].channel   = 2; //
    leds->ledbar_static[1][2].device_no = 0; //
    leds->ledbar_static[1][2].channel   = 3; //
    leds->ledbar_static[2][2].device_no = 0; //
    leds->ledbar_static[2][2].channel   = 4; //
    leds->ledbar_static[3][2].device_no = 0; //
    leds->ledbar_static[3][2].channel   = 5; //
    leds->ledbar_static[4][2].device_no = 0; //
    leds->ledbar_static[4][2].channel   = 6; //

    leds->ledbar_static[0][3].device_no = 3; //
    leds->ledbar_static[0][3].channel   = 0; //
    leds->ledbar_static[1][3].device_no = 3; //
    leds->ledbar_static[1][3].channel   = 1; //
    leds->ledbar_static[2][3].device_no = 3; //
    leds->ledbar_static[2][3].channel   = 2; //
    leds->ledbar_static[3][3].device_no = 3; //
    leds->ledbar_static[3][3].channel   = 3; //
    leds->ledbar_static[4][3].device_no = 3; //
    leds->ledbar_static[4][3].channel   = 4; //

    leds->ledbar_static[0][4].device_no = 3; //
    leds->ledbar_static[0][4].channel   = 5; //
    leds->ledbar_static[1][4].device_no = 3; //
    leds->ledbar_static[1][4].channel   = 6; //
    leds->ledbar_static[2][4].device_no = 3; //
    leds->ledbar_static[2][4].channel   = 7; //
    leds->ledbar_static[3][4].device_no = 1; //
    leds->ledbar_static[3][4].channel   = 0; //
    leds->ledbar_static[4][4].device_no = 1; //
    leds->ledbar_static[4][4].channel   = 1; //

    leds->ledbar_static[0][5].device_no = 1; //
    leds->ledbar_static[0][5].channel   = 2; //
    leds->ledbar_static[1][5].device_no = 1; //
    leds->ledbar_static[1][5].channel   = 3; //
    leds->ledbar_static[2][5].device_no = 1; //
    leds->ledbar_static[2][5].channel   = 4; //
    leds->ledbar_static[3][5].device_no = 1; //
    leds->ledbar_static[3][5].channel   = 5; //
    leds->ledbar_static[4][5].device_no = 1; //
    leds->ledbar_static[4][5].channel   = 6; //


    // Initialize the led bards that are on the switches, not placed in a for loop so they can be calibrated
    // idividually
    /*0-4*/
    led_controller_add_led_switch_device(&leds->ledbar_switch[0][0], 0x43, 0);
    led_controller_add_led_switch_device(&leds->ledbar_switch[0][1], 0x43, 1);
    led_controller_add_led_switch_device(&leds->ledbar_switch[0][2], 0x43, 2);
    led_controller_add_led_switch_device(&leds->ledbar_switch[0][3], 0x43, 3);
    led_controller_add_led_switch_device(&leds->ledbar_switch[0][4], 0x43, 4);
    /*5-9*/
    led_controller_add_led_switch_device(&leds->ledbar_switch[1][0], 0x43, 5);
    led_controller_add_led_switch_device(&leds->ledbar_switch[1][1], 0x43, 6);
    led_controller_add_led_switch_device(&leds->ledbar_switch[1][2], 0x43, 7);
    led_controller_add_led_switch_device(&leds->ledbar_switch[1][3], 0x43, 8);
    led_controller_add_led_switch_device(&leds->ledbar_switch[1][4], 0x43, 9);
    /*10-14*/
    led_controller_add_led_switch_device(&leds->ledbar_switch[2][0], 0x43, 10);
    led_controller_add_led_switch_device(&leds->ledbar_switch[2][1], 0x43, 11);
    led_controller_add_led_switch_device(&leds->ledbar_switch[2][2], 0x43, 12);
    led_controller_add_led_switch_device(&leds->ledbar_switch[2][3], 0x43, 13);
    led_controller_add_led_switch_device(&leds->ledbar_switch[2][4], 0x43, 14);
    /*15-19*/
    led_controller_add_led_switch_device(&leds->ledbar_switch[3][0], 0x41, 0);
    led_controller_add_led_switch_device(&leds->ledbar_switch[3][1], 0x41, 1);
    led_controller_add_led_switch_device(&leds->ledbar_switch[3][2], 0x41, 3);
    led_controller_add_led_switch_device(&leds->ledbar_switch[3][3], 0x41, 2);
    led_controller_add_led_switch_device(&leds->ledbar_switch[3][4], 0x41, 4);
    /*20-24*/
    led_controller_add_led_switch_device(&leds->ledbar_switch[4][0], 0x41, 5);
    led_controller_add_led_switch_device(&leds->ledbar_switch[4][1], 0x41, 6);
    led_controller_add_led_switch_device(&leds->ledbar_switch[4][2], 0x41, 7);
    led_controller_add_led_switch_device(&leds->ledbar_switch[4][3], 0x41, 8);
    led_controller_add_led_switch_device(&leds->ledbar_switch[4][4], 0x41, 9);


    for (int row = 0; row < NO_OF_STATIC_LEDBARS_LINES; row++) {
        for (int col = 0; col < NO_OF_STATIC_LEDBARS_PER_LINE; col++) {
            leds->ledbar_static[row][col].state = lamp_state_off;
        }
    }


    led_controller_init(millis_wrapper, led_bar_set_pwm_duty_cycle, led_bar_static_set_state, led_lamp_set_state, leds);
    digitalWrite(LED_BAR_STATIC_SER_OE, LOW);
    digitalWrite(LED_LAMP_OE, LOW);
}

void initializePca9685() {
    int device_frequency;
    Wire.setPins(SDA_PIN, SCL_PIN);
    Wire.begin();
    pca9685.setWire(Wire);
    pca9685.addDevice(0x40);
    pca9685.addDevice(0x41);
    pca9685.addDevice(0x42);
    pca9685.addDevice(0x43);
    // pca9685.setupOutputEnablePin(constants::output_enable_pin); //ΤΟΔΟ
    pca9685.setSingleDeviceToFrequency(0x40, 50);
    pca9685.setSingleDeviceToFrequency(0x41, 2000);
    pca9685.setSingleDeviceToFrequency(0x42, 50);
    pca9685.setSingleDeviceToFrequency(0x43, 2000);

    // Verify device connection
    device_frequency = pca9685.getSingleDeviceFrequency(0x40);
    Serial.print("Freq A: ");
    Serial.println(device_frequency);
    delay(100);
    device_frequency = pca9685.getSingleDeviceFrequency(0x41);
    Serial.print("Freq B: ");
    Serial.println(device_frequency);
    delay(100);
    device_frequency = pca9685.getSingleDeviceFrequency(0x42);
    Serial.print("Freq C: ");
    Serial.println(device_frequency);
    delay(100);
    device_frequency = pca9685.getSingleDeviceFrequency(0x43);
    Serial.print("Freq D: ");
    Serial.println(device_frequency);
    delay(100);
    LOG_INFO("pca9685 set");
}

void drawLevel(map_t* map) {
    static long flash_on_timestamp;
    static int red_binded_switch_col;
    static int red_binded_switch_line;
    static int red_binded_switch_blink_flag;
    int activeSegments[5][6] = {0};
    int start_line           = -1;

    for (int i = 0; i < NO_OF_3_WAY_LINES; ++i) {
        if (map->start_nodes[i] != 0) {
            start_line = i;
        }
    }
    markActiveSegments(activeSegments, map->switches, start_line);
    // draw the leds
    for (int lineIndex = 0; lineIndex < NO_OF_STATIC_LEDBARS_LINES; ++lineIndex) {


        
        for (int columnIndex = 0; columnIndex < NO_OF_STATIC_LEDBARS_PER_LINE; ++columnIndex) {

            leds.ledbar_static[lineIndex][columnIndex].state = (activeSegments[lineIndex][columnIndex] == 1) ? lamp_state_on : lamp_state_off;
        }
        for (int columnIndex = 0; columnIndex < NO_OF_SWITCH_LEDS_PER_LINE; ++columnIndex) {
            servos[lineIndex * 5 + columnIndex].position =switch_pos_to_servo_pos(map->switches[lineIndex][columnIndex].position);
            if ((map->switches[lineIndex][columnIndex].selected)) {
                
                
                if (change_position_flag) { 
                    change_position_flag = false;
                }
                // at rotary encoder position make it blinking
                leds.ledbar_switch[columnIndex][lineIndex].state = lamp_state_blink;

                 if(map->switches[lineIndex][columnIndex].switch_color==red)
                {               
                    red_binded_switch_col = map->switches[lineIndex][columnIndex].binded_switch_index.column;
                    red_binded_switch_line= map->switches[lineIndex][columnIndex].binded_switch_index.line;
                    red_binded_switch_blink_flag=1;               
                } 
                else
                {
                    red_binded_switch_blink_flag=0;
                }                           
            } else {
                leds.ledbar_switch[columnIndex][lineIndex].state =(map->switches[lineIndex][columnIndex].has_power) ? lamp_state_on : lamp_state_off;
                 
 
                 if(red_binded_switch_blink_flag)
                {
                    leds.ledbar_switch[red_binded_switch_col][red_binded_switch_line].state = lamp_state_blink;         
                }
            }
        }
        leds.led_lamp[lineIndex][0].state = (map->start_nodes[lineIndex] != 0) ? lamp_state_on : lamp_state_off;
        leds.led_lamp[lineIndex][1].state = (lineIndex == map->line_end_goal) ? lamp_state_on : lamp_state_off;
    }
 

    
    while(servo_ctrl_update(servos));
    led_controller_update(&leds);
}

void markActiveSegments(
    int Z[NO_OF_3_WAY_LINES][NO_OF_SWITCHES_PER_LINE + 1], const three_way_switches_array_t switches, int8_t startx) {
    Z[startx][0] = 1;
    int newk;
    int k = startx;
    for (int c = 1; c < NO_OF_SWITCHES_PER_LINE + 1; ++c) {
        newk = k;
        switch (switches[k][c - 1].position) {
        case mid_switch:
            {
                Z[k][c] = Z[k][c - 1];
                newk    = k;
                break;
            }
        case low_switch:
            if (k + 1 < 5) {
                Z[k + 1][c] = Z[k][c - 1];
                newk        = k + 1;
            }
            break;
        case high_switch:
            if (k - 1 >= 0) {
                Z[k - 1][c] = Z[k][c - 1];
                newk        = k - 1;
            }
            break;
        default:
            LOG_ERROR("No such switch");
        }
        k = newk;
    }
}


void appendInfo(const int end_goal, const int time_left,unsigned char numberOfMovements, const int current_level) {
    unsigned int max_time = switches_time_get_level_time(current_level);
    time_display_set_time((unsigned int) time_left, max_time);
    nixie_controller_diplay_number(numberOfMovements);
    // sound_module.goToTrack(current_level / 5);
}


void level_win(void)
{
    led_controller_victory_round(&leds);
}

void get_controls_status(rotary_enc_t* rotary) {
    rotary_t rotary_status;
    Rotary_get_status(&rotary_status);
    rotary->direction = rotary_status.direction;
    rotary->button    = rotary_status.button;
    if (rotary->button) {
        change_position_flag = true;
    }
    // if no button played for a long time stop playing sound ???
}


void init_level(unsigned char current_level)
{
    for (int lineIndex = 0; lineIndex < NO_OF_STATIC_LEDBARS_LINES; ++lineIndex) {
        for (int columnIndex = 0; columnIndex < NO_OF_STATIC_LEDBARS_PER_LINE; ++columnIndex) {

            leds.ledbar_static[lineIndex][columnIndex].state = lamp_state_off;
        }
        for (int columnIndex = 0; columnIndex < NO_OF_SWITCH_LEDS_PER_LINE; ++columnIndex) {
            servos[lineIndex * 5 + columnIndex].position =servo_pos_center;
            leds.ledbar_switch[lineIndex][columnIndex].state=lamp_state_off;
        }
        leds.led_lamp[lineIndex][0].state = lamp_state_off;
        leds.led_lamp[lineIndex][1].state = lamp_state_off;
    }
    
    led_controller_update(&leds);
    nixie_controller_diplay_number(NIXIE_OFF);
    for(int i=0; i<100; i++)
    {
        time_display_set_time( i, 100); //move the needle slowly to 100%
        delay(20);
    }
    while(servo_ctrl_update(servos));
    nixie_controller_flash_number(current_level+1);//current level starts from 0
}

void shutDownDevice() {
    // YOLO (literaly): No shutdown procedure
}

void init_mui_structures(userInterface_t** gui) {
    *gui                        = (userInterface_t*) malloc(sizeof(userInterface_t));
    (*gui)->initVisuals         = initVisuals;
    (*gui)->initControls        = initRotaryEncoder;
    (*gui)->RandomNumberSeed    = random_number_seed;
    (*gui)->drawLevel           = drawLevel;
    (*gui)->appendInfo          = appendInfo;
    (*gui)->init_level          = init_level;
    (*gui)->get_controls_status = get_controls_status;
    (*gui)->terminate           = shutDownDevice;
    (*gui)->level_win           = level_win;
}


void test_peripherals() {
    sound_module.test();
    led_controller_test(&leds);
    nixie_controller_test();

    for (int i = 0; i < 100; ++i) {
        time_display_set_time(i, 100);
        delay(50);
    }
    servo_ctrl_test(servos);
}

void delete_mui_structures(userInterface_t** gui) {
    free(*gui);
    *gui = NULL;
}
