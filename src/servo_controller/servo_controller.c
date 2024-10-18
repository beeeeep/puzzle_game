#ifdef __cplusplus
extern "C" {
#endif


#include "servo_controller.h"

static milliseconds_t millis_func;

static servo_ctrl_set_pwm_t servo_ctrl_set_pwm_func;


void servo_ctrl_init(milliseconds_t millis_func_p, servo_ctrl_set_pwm_t servo_ctrl_set_pwm_func_p) {
    millis_func             = millis_func_p;
    servo_ctrl_set_pwm_func = servo_ctrl_set_pwm_func_p;
}

void servo_ctrl_add_device(servo_motor_t servos[NO_OF_SERVOS], unsigned char device_index, unsigned int pc9685_id,
    unsigned char channel, unsigned int high_pos_value, unsigned int center_pos_value, unsigned int low_pos_value) {
    servos[device_index].pc9685_id        = pc9685_id;
    servos[device_index].channel          = channel;
    servos[device_index].high_pos_value   = high_pos_value;
    servos[device_index].center_pos_value = center_pos_value;
    servos[device_index].low_pos_value    = low_pos_value;
    servos[device_index].ready            = 0;
    servos[device_index].position         = servo_pos_undefined;
    servos[device_index].position_prev    = servo_pos_undefined;
}

void servo_ctrl_change_position(servo_motor_t* servo) {
    unsigned int pwm_value = 0;

    switch (servo->position) {
    case servo_pos_high:
        pwm_value = servo->high_pos_value;
        break;
    case servo_pos_center:
        pwm_value = servo->center_pos_value;
        break;
    case servo_pos_low:
        pwm_value = servo->low_pos_value;
        break;
    }
    servo_ctrl_set_pwm_func(servo->pc9685_id, servo->channel, pwm_value);
}

void servo_ctrl_update(servo_motor_t servos[NO_OF_SERVOS]) {
    static unsigned long timestamp;
    static unsigned char servo_queue[NO_OF_SERVOS];
    static unsigned char tail_index;
    static unsigned char head_index;

    // check for new directives
    for (int i = 0; i < NO_OF_SERVOS; i++) {
        if (servos[i].position != servos[i].position_prev) {
            // que position update
            servos[i].ready         = 0;
            servo_queue[head_index] = i;
            head_index              = (head_index < (NO_OF_SERVOS - 1)) ? head_index + 1 : 0;
        }
        servos[i].position_prev = servos[i].position;
    }

    // Check if any motor needs to change angle
    if (head_index != tail_index) {
        if (millis_func() - timestamp
            > SERVO_UPDATE_RATE_MS) // execute queued movement when the time passes, so not every servo moves at once
        {
            servo_ctrl_change_position(&servos[servo_queue[tail_index]]);
            servos[servo_queue[tail_index]].ready = 1;

            tail_index = (tail_index < (NO_OF_SERVOS - 1)) ? tail_index + 1 : 0;
            timestamp  = millis_func();
        }
    }
}
#ifdef __cplusplus
}
#endif
