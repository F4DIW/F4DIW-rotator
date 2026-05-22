#ifndef LIBRARIES_GLOBALS_H_
#define LIBRARIES_GLOBALS_H_

#include <Arduino.h>

enum _rotator_status { idle = 1, moving = 2, pointing = 4, error = 8 };
enum _rotator_error { no_error = 1, sensor_error = 2, homing_error = 4,
                      motor_error = 8, over_temperature = 12, wdt_error = 16 };
enum _control_mode { position = 0, speed = 1 };

struct _control {
    double input, input_prv, speed, setpoint, setpoint_speed;
    uint16_t load;
    double u, p, i, d;
};

struct _rotator {
    volatile enum _rotator_status rotator_status;
    volatile enum _rotator_error rotator_error;
    enum _control_mode control_mode;
    bool homing_flag;
    int8_t inside_temperature;
    double park_az, park_el;
    uint8_t fault_az, fault_el;
    bool switch_az, switch_el;
};

extern _control control_az;
extern _control control_el;
extern _rotator rotator;

#endif
