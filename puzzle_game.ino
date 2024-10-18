#include "PCA9685.h"
#include <Wire.h>
#include "src/servo_controller/servo_controller.h"
#include "src/rotary/rotary.h"
#include "src/led_controller/led_controller.h"

#define ROTARY_A_PIN 10
#define ROTARY_B_PIN 11
#define ROTARY_BUTTON_PIN 12
#define SCL_PIN 4
#define SDA_PIN 5 
#define PCA9685_OUTPUT_ENABLE_PIN
#define NIXIE_LATCH_PIN 8
#define NIXIE_CLOCK_PIN 3
#define NIXIE_DATA_PIN 18

#define LED_BAR_STATIC_SER_RCLK 16
#define LED_BAR_STATIC_SER_SCLK 17
#define LED_BAR_STATIC_SER_OE 8  
#define LED_BAR_STATIC_SER_0 7
#define LED_BAR_STATIC_SER_1 5
#define LED_BAR_STATIC_SER_2 18
#define LED_BAR_STATIC_SER_3 15

#define LED_LAMP_SCLK 47
#define LED_LAMP_RCLK 21
#define LED_LAMP_OE 48 
#define LED_LAMP_SER 15

PCA9685 pca9685;
servo_motor_t servos[NO_OF_SERVOS];
leds_ctrl_str_t leds;

int nixie_data[16]-=={64,32,16,8,4,2,1,32768,16384,8192};


//WRAPPER FUNCTIONS
void setDeviceChannelServoPulseDuration_wrapper(unsigned char cntl_addr, unsigned int servo_channel, unsigned int pos)
{
  pca9685.setDeviceChannelServoPulseDuration(cntl_addr,servo_channel, pos);
}

void led_bar_set_pwm_duty_cycle(unsigned int device_ID, unsigned char channel, unsigned int power)
{
  pca9685.setAllDeviceChannelsDutyCycle(device_ID,channel,power);
}


void led_bar_static_set_state(unsigned char device_no, unsigned char channel, unsigned char state)
{

}

void led_lamp_static_set_state(unsigned char device_no, unsigned char channel, unsigned char state)
{

}

void nixie_set_number(unsigned char number)
{
  shiftOut(NIXIE_DATA_PIN, NIXIE_CLOCK_PIN, MSBFIST, 0xFF & (nixie_data[number]>>8));
  shiftOut(NIXIE_DATA_PIN, NIXIE_CLOCK_PIN, MSBFIST, 0xFF & nixie_data[number]);
}


unsigned long millis_wrapper(void)
{
  return (unsigned long)millis();
}

unsigned char rotary_read_pin_A(void)
{
 return (unsigned char)digitalRead(ROTARY_A_PIN);
}

unsigned char rotary_read_pin_B(void)
{
 return (unsigned char)digitalRead(ROTARY_B_PIN);
}

unsigned char rotary_read_button(void)
{
 return (unsigned char)digitalRead(ROTARY_BUTTON_PIN);
}


void setup() 
{
  //********************* PCA9685 Init START *********************
  int device_frequency;
  Wire.setPins(SDA_PIN, SCL_PIN);
  Wire.begin();
  pca9685.setWire(Wire);
  pca9685.addDevice(0x40);
  pca9685.addDevice(0x41);
  pca9685.addDevice(0x42);
  pca9685.addDevice(0x43);
  //pca9685.setupOutputEnablePin(constants::output_enable_pin); //ΤΟΔΟ
  pca9685.setSingleDeviceToFrequency(0x40, 50);
  pca9685.setSingleDeviceToFrequency(0x41, 50);
  pca9685.setSingleDeviceToFrequency(0x42, 50);
  pca9685.setSingleDeviceToFrequency(0x43, 50);

  //Verify device connection
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
  //********************* PCA9685 Init END *********************

  //Init Servos
  servo_ctrl_init(millis_wrapper,setDeviceChannelServoPulseDuration_wrapper);
  servo_ctrl_add_device(servos, 0, 0x40,0, 2000, 1500,  800);
  servo_ctrl_add_device(servos, 1, 0x40,1, 2000, 1500,  800);
  servo_ctrl_add_device(servos, 2, 0x40,2, 2000, 1500,  800);
  servo_ctrl_add_device(servos, 3, 0x40,3, 2000, 1500,  800);
  servo_ctrl_add_device(servos, 4, 0x40,4, 2000, 1500,  800);
  servo_ctrl_add_device(servos, 5, 0x40,5, 2000, 1500,  800);
  servo_ctrl_add_device(servos, 6, 0x40,6, 2000, 1500,  800);
  servo_ctrl_add_device(servos, 7, 0x40,7, 2000, 1500,  800);
  servo_ctrl_add_device(servos, 8, 0x40,8, 2000, 1500,  800);
  servo_ctrl_add_device(servos, 9, 0x40,9, 2000, 1500,  800);
  servo_ctrl_add_device(servos, 10, 0x40,10, 2000, 1500, 800);
  servo_ctrl_add_device(servos, 11, 0x40,11, 2000, 1500, 800);
  servo_ctrl_add_device(servos, 12, 0x40,12, 2000, 1500, 800);
  servo_ctrl_add_device(servos, 13, 0x40,13, 2000, 1500, 800);
  servo_ctrl_add_device(servos, 14, 0x40,14, 2000, 1500, 800);
  servo_ctrl_add_device(servos, 15, 0x40,15, 2000, 1500, 800);
  servo_ctrl_add_device(servos, 16, 0x41,0, 2000, 1500,  800);
  servo_ctrl_add_device(servos, 17, 0x41,1, 2000, 1500,  800);
  servo_ctrl_add_device(servos, 18, 0x41,2, 2000, 1500,  800);
  servo_ctrl_add_device(servos, 19, 0x41,3, 2000, 1500,  800);
  servo_ctrl_add_device(servos, 20, 0x41,4, 2000, 1500,  800);
  servo_ctrl_add_device(servos, 21, 0x41,5, 2000, 1500,  800);
  servo_ctrl_add_device(servos, 22, 0x41,6, 2000, 1500,  800);
  servo_ctrl_add_device(servos, 23, 0x41,7, 2000, 1500,  800);
  servo_ctrl_add_device(servos, 24, 0x41,8, 2000, 1500,  800);

  //********************* Rotary encoder init START *********************
  pinMode(ROTARY_A_PIN, INPUT_PULLUP);
  pinMode(ROTARY_B_PIN, INPUT_PULLUP);
  pinMode(ROTARY_BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(ROTARY_A_PIN, Rotary_AB_pin_callback_fuction, FALLING);
  attachInterrupt(ROTARY_B_PIN, Rotary_AB_pin_callback_fuction, FALLING);
  attachInterrupt(ROTARY_BUTTON_PIN, Rotary_button_pin_callback_fuction, FALLING);
  Rotary_init(rotary_read_pin_A, rotary_read_pin_B, rotary_read_button, millis_wrapper);
  //********************* Rotary encoder init END  *********************

  //********************* LED controller init START *********************
   led_controller_init(millis_wrapper, led_bar_set_pwm_duty_cycle, led_bar_static_set_state, led_lamp_static_set_state);


  //********************* LED controller init END *********************


}


void loop() {
  // put your main code here, to run repeatedly:

  Rotary_get_status(&rotary);
  servo_ctrl_update(servos);
  led_controller_update(&leds);
}
