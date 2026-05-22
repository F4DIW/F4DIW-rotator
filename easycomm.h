#ifndef EASYCOMM_H_
#define EASYCOMM_H_

#include <Arduino.h>
#include "BluetoothSerial.h"
#include "globals.h"

#define BUFFER_SIZE 256
#define BT_DEVICE_NAME "SatNOGS-Rotator"

typedef enum { SOURCE_USB, SOURCE_BT } _comm_source;

extern BluetoothSerial SerialBT;

class easycomm {
public:
    void easycomm_init() {
        Serial.begin(9600);
        SerialBT.begin(BT_DEVICE_NAME);
        Serial.println("[INFO] EasyComm II ready - USB + BT");
    }

    void easycomm_proc() {
        if (SerialBT.available()) {
            _active_source = SOURCE_BT;
            _read_and_process(SerialBT);
        }
        if (Serial.available()) {
            _active_source = SOURCE_USB;
            _read_and_process(Serial);
        }
    }

private:
    _comm_source _active_source = SOURCE_USB;
    char _buffer[BUFFER_SIZE];
    uint16_t _buf_cnt = 0;

    void _read_and_process(Stream &src) {
        while (src.available()) {
            char c = (char)src.read();
            if (c == '\n' || c == '\r') {
                if (_buf_cnt > 0) {
                    _buffer[_buf_cnt] = '\0';
                    _process_cmd(_buffer);
                    _buf_cnt = 0;
                }
            } else {
                if (_buf_cnt < BUFFER_SIZE - 1)
                    _buffer[_buf_cnt++] = c;
            }
        }
    }

    void _send(String response) {
        if (_active_source == SOURCE_BT) SerialBT.print(response);
        Serial.print(response);
    }

    void _process_cmd(char *cmd) {
        String str = String(cmd);
        if (str.startsWith("AZ")) {
            if (str.length() > 2) {
                control_az.setpoint = constrain(str.substring(2).toFloat(), MIN_M1_ANGLE, MAX_M1_ANGLE);
            } else { _send("AZ" + String(control_az.input, 1) + "\n"); }
        } else if (str.startsWith("EL")) {
            if (str.length() > 2) {
                control_el.setpoint = constrain(str.substring(2).toFloat(), MIN_M2_ANGLE, MAX_M2_ANGLE);
            } else { _send("EL" + String(control_el.input, 1) + "\n"); }
        } else if (str.startsWith("SA")) {
            control_az.setpoint = control_az.input;
        } else if (str.startsWith("SE")) {
            control_el.setpoint = control_el.input;
        } else if (str.startsWith("GS")) {
            _send("GS" + String(rotator.rotator_status, DEC) + "\n");
        } else if (str.startsWith("GE")) {
            _send("GE" + String(rotator.rotator_error, DEC) + "\n");
        } else if (str.startsWith("VE")) {
            _send("VE001\n");
        } else if (str.startsWith("IP")) {
            _send("AZ" + String(control_az.input, 1) + " EL" + String(control_el.input, 1) + "\n");
        } else if (str.startsWith("RB")) {
            delay(100); ESP.restart();
        }
    }
};

#endif
