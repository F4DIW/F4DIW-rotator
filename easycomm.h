#ifndef EASYCOMM_H_
#define EASYCOMM_H_

#include <Arduino.h>
#include "BluetoothSerial.h"
#include "esp_bt_device.h"
#include "globals.h"

#define BUFFER_SIZE 256
#define BT_DEVICE_NAME "SatNOGS-Rotator"

typedef enum { SOURCE_USB, SOURCE_BT } _comm_source;

extern BluetoothSerial SerialBT;

class easycomm {
public:
    void easycomm_init() {
        Serial.begin(9600);
        delay(500);

        SerialBT.register_callback([](esp_spp_cb_event_t event, esp_spp_cb_param_t* param) {
            if (event == ESP_SPP_SRV_OPEN_EVT) Serial.println("[BT] Connected");
            if (event == ESP_SPP_CLOSE_EVT)    Serial.println("[BT] Disconnected");
        });

        SerialBT.begin(BT_DEVICE_NAME);

        const uint8_t* mac = esp_bt_dev_get_address();
        char macStr[18];
        snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        Serial.println("[INFO] EasyComm ready - USB + BT");
        Serial.print("[INFO] BT Name: ");
        Serial.println(BT_DEVICE_NAME);
        Serial.print("[INFO] BT MAC:  ");
        Serial.println(macStr);
    }

    void easycomm_proc() {
        // Envoyer position toutes les secondes vers BT
        static uint32_t last_report = 0;
        if (millis() - last_report > 1000) {
            String pos = "AZ" + String(control_az.setpoint, 1) +
                         " EL" + String(control_el.setpoint, 1) + "\n";
            SerialBT.print(pos);
            Serial.print("[TX] " + pos);
            last_report = millis();
        }

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
                // Fin de trame normale
                if (_buf_cnt > 0) {
                    _buffer[_buf_cnt] = '\0';
                    _process_cmd(_buffer);
                    _buf_cnt = 0;
                }
            } else if (c == 'P' && _buf_cnt > 0) {
                // Look4Sat envoie sans \n entre les trames
                // On detecte le debut d un nouveau paquet P
                _buffer[_buf_cnt] = '\0';
                _process_cmd(_buffer);
                _buf_cnt = 0;
                _buffer[_buf_cnt++] = c;
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
        str.trim();

        // Look4Sat format: "P az el"
        if (str.startsWith("P ")) {
            int space1 = str.indexOf(' ');
            int space2 = str.indexOf(' ', space1 + 1);
            if (space1 >= 0 && space2 >= 0) {
                control_az.setpoint = constrain(
                    str.substring(space1 + 1, space2).toFloat(),
                    MIN_M1_ANGLE, MAX_M1_ANGLE);
                control_el.setpoint = constrain(
                    str.substring(space2 + 1).toFloat(),
                    MIN_M2_ANGLE, MAX_M2_ANGLE);
                Serial.println("[L4S] AZ=" + String(control_az.setpoint, 1) +
                               " EL=" + String(control_el.setpoint, 1));
            }
            return;
        }

        // EasyComm I : "AZxxx.x ELxxx.x" sur une seule ligne
        int az_idx = str.indexOf("AZ");
        int el_idx = str.indexOf("EL");
        if (az_idx >= 0 && el_idx >= 0) {
            control_az.setpoint = constrain(
                str.substring(az_idx + 2, el_idx).toFloat(),
                MIN_M1_ANGLE, MAX_M1_ANGLE);
            control_el.setpoint = constrain(
                str.substring(el_idx + 2).toFloat(),
                MIN_M2_ANGLE, MAX_M2_ANGLE);
            return;
        }

        // EasyComm II : commandes separees
        if (str.startsWith("AZ")) {
            if (str.length() > 2)
                control_az.setpoint = constrain(str.substring(2).toFloat(), MIN_M1_ANGLE, MAX_M1_ANGLE);
            else
                _send("AZ" + String(control_az.input, 1) + "\n");
        } else if (str.startsWith("EL")) {
            if (str.length() > 2)
                control_el.setpoint = constrain(str.substring(2).toFloat(), MIN_M2_ANGLE, MAX_M2_ANGLE);
            else
                _send("EL" + String(control_el.input, 1) + "\n");
        } else if (str.startsWith("AZ EL")) {
            _send("AZ" + String(control_az.input, 1) +
                  " EL" + String(control_el.input, 1) + "\n");
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
            _send("AZ" + String(control_az.input, 1) +
                  " EL" + String(control_el.input, 1) + "\n");
        } else if (str.startsWith("RB")) {
            delay(100);
            ESP.restart();
        }
    }
};

#endif /* EASYCOMM_H_ */
