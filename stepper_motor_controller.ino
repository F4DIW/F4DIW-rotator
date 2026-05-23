#define SAMPLE_TIME        0.1
#define RATIO              19.2
#define MICROSTEP          8
#define MIN_PULSE_WIDTH    20
#define MAX_SPEED          3200
#define MAX_ACCELERATION   1600
#define SPR                1600L
#define MIN_M1_ANGLE       0
#define MAX_M1_ANGLE       360
#define MIN_M2_ANGLE       0
#define MAX_M2_ANGLE       180
#define DEFAULT_HOME_STATE LOW
#define HOME_DELAY         12000
#define SKIP_HOMING        1

#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

#include "AccelStepper.h"
#include "Wire.h"
#include "globals.h"
#include "easycomm.h"
#include "cnc_shield_rotator_pins.h"
#include "endstop.h"

_control control_az = { 0, 0, 0, 0, 0, 0, 0, 8.0, 0.0, 0.5 };
_control control_el = { 0, 0, 0, 0, 0, 0, 0, 10.0, 0.0, 0.3 };
_rotator rotator    = { idle, no_error, position, false, 0, 0, 0, LOW, LOW, false, false };

uint32_t t_run = 0;
easycomm comm;
AccelStepper stepper_az(1, M1IN1, M1IN2);
AccelStepper stepper_el(1, M2IN1, M2IN2);
endstop switch_az(SW1, DEFAULT_HOME_STATE), switch_el(SW2, DEFAULT_HOME_STATE);

enum _rotator_error homing(int32_t seek_az, int32_t seek_el);
int32_t deg2step(float deg);
float step2deg(int32_t step);

void setup() {
    switch_az.init();
    switch_el.init();
    comm.easycomm_init();
    stepper_az.setEnablePin(MOTOR_EN);
    stepper_az.setPinsInverted(true, false, true);  // DIR inverted for CNC shield
    stepper_az.enableOutputs();
    stepper_az.setMaxSpeed(MAX_SPEED);
    stepper_az.setAcceleration(MAX_ACCELERATION);
    stepper_az.setMinPulseWidth(MIN_PULSE_WIDTH);
    stepper_el.setPinsInverted(true, false, true);  // DIR inverted for CNC shield
    stepper_el.enableOutputs();
    stepper_el.setMaxSpeed(MAX_SPEED);
    stepper_el.setAcceleration(MAX_ACCELERATION);
    stepper_el.setMinPulseWidth(MIN_PULSE_WIDTH);
#if SKIP_HOMING == 1
    rotator.homing_flag = true;
    rotator.rotator_status = idle;
    rotator.rotator_error = no_error;
    Serial.println("[INFO] Homing SKIPPED");
#endif
}

void loop() {
    yield();
    rotator.switch_az = switch_az.get_state();
    rotator.switch_el = switch_el.get_state();
    comm.easycomm_proc();
    control_az.input = step2deg(stepper_az.currentPosition());
    control_el.input = step2deg(stepper_el.currentPosition());
    if (rotator.rotator_status != error) {
        if (rotator.homing_flag == false) {
            rotator.control_mode = position;
            rotator.rotator_error = homing(deg2step(-MAX_M1_ANGLE), deg2step(-MAX_M2_ANGLE));
            if (rotator.rotator_error == no_error) {
                rotator.rotator_status = idle;
                rotator.homing_flag = true;
            } else {
                rotator.rotator_status = error;
                rotator.rotator_error = homing_error;
            }
        } else {
            stepper_az.moveTo(deg2step(control_az.setpoint));
            stepper_el.moveTo(deg2step(control_el.setpoint));
            rotator.rotator_status = pointing;
            stepper_az.run();
            stepper_el.run();
            if (stepper_az.distanceToGo() == 0 && stepper_el.distanceToGo() == 0)
                rotator.rotator_status = idle;
        }
    } else {
        stepper_az.stop();
        stepper_az.disableOutputs();
        stepper_el.stop();
        stepper_el.disableOutputs();
        if (rotator.rotator_error != homing_error) {
            rotator.rotator_error = no_error;
            rotator.rotator_status = idle;
        }
    }
}

enum _rotator_error homing(int32_t seek_az, int32_t seek_el) {
    bool isHome_az = false;
    bool isHome_el = false;
    stepper_az.moveTo(seek_az);
    stepper_el.moveTo(seek_el);
    while (isHome_az == false || isHome_el == false) {
        yield();
        if (switch_az.get_state() == true && !isHome_az) {
            stepper_az.moveTo(stepper_az.currentPosition());
            isHome_az = true;
        }
        if (switch_el.get_state() == true && !isHome_el) {
            stepper_el.moveTo(stepper_el.currentPosition());
            isHome_el = true;
        }
        if ((stepper_az.distanceToGo() == 0 && !isHome_az) ||
            (stepper_el.distanceToGo() == 0 && !isHome_el))
            return homing_error;
        stepper_az.run();
        stepper_el.run();
    }
    uint32_t time = millis();
    while (millis() - time < HOME_DELAY) {
        yield();
        stepper_az.run();
        stepper_el.run();
    }
    stepper_az.setCurrentPosition(0);
    stepper_el.setCurrentPosition(0);
    control_az.setpoint = 0;
    control_el.setpoint = 0;
    return no_error;
}

int32_t deg2step(float deg) {
    return (RATIO * SPR * deg / 360);
}

float step2deg(int32_t step) {
    return (360.00 * step / (SPR * RATIO));
}