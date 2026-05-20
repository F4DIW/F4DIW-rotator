/*!
 * @file easycomm.h
 *
 * EasyComm II protocol implementation for SatNOGS rotator
 * with dual connection: USB Serial + Bluetooth (ESP32)
 *
 * Compatible with Look4Sat (Bluetooth SPP) and Gpredict (USB Serial)
 *
 * Licensed under the GPLv3
 */

#ifndef EASYCOMM_H_
#define EASYCOMM_H_

#include <Arduino.h>
#include "BluetoothSerial.h"
#include "globals.h"

#define BUFFER_SIZE 256  ///< Serial buffer size

// Bluetooth device name visible in Look4Sat / Android
#define BT_DEVICE_NAME "SatNOGS-Rotator"

// Active communication source
typedef enum {
    SOURCE_USB,
    SOURCE_BT
} _comm_source;

// Global BluetoothSerial instance (accessible from watchdog if needed)
BluetoothSerial SerialBT;

/**************************************************************************/
/*!
    @brief    Class implementing EasyComm II protocol over USB Serial
              and Bluetooth Serial simultaneously (ESP32).
              - Commands are accepted from whichever interface sends first.
              - Responses are sent to the active source.
              - USB Serial always receives debug/monitor output.
*/
/**************************************************************************/
class easycomm {
public:

    /**************************************************************************/
    /*!
        @brief    Initialize USB Serial and Bluetooth Serial
    */
    /**************************************************************************/
    void easycomm_init() {
        // USB Serial for debug and Gpredict/rotcld
        Serial.begin(9600);
        // Bluetooth Classic SPP for Look4Sat
        SerialBT.begin(BT_DEVICE_NAME);
        Serial.println("[INFO] EasyComm II ready - USB + BT (" BT_DEVICE_NAME ")");
    }

    /**************************************************************************/
    /*!
        @brief    Process incoming EasyComm II commands from USB or BT.
                  Call this in the main loop.
    */
    /**************************************************************************/
    void easycomm_proc() {
        // Determine active source (BT has priority if both have data)
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

    /**************************************************************************/
    /*!
        @brief    Read bytes from a Stream, accumulate into buffer, and
                  process complete lines terminated by \n or \r
        @param    src   Reference to Serial or SerialBT stream
    */
    /**************************************************************************/
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
                if (_buf_cnt < BUFFER_SIZE - 1) {
                    _buffer[_buf_cnt++] = c;
                }
            }
        }
    }

    /**************************************************************************/
    /*!
        @brief    Send a response string to the active source.
                  Always mirrors to USB for monitoring.
        @param    response  String to send
    */
    /**************************************************************************/
    void _send(String response) {
        if (_active_source == SOURCE_BT) {
            SerialBT.print(response);
        }
        // Always output on USB for debug/monitoring
        Serial.print(response);
    }

    /**************************************************************************/
    /*!
        @brief    Parse and execute an EasyComm II command
        @param    cmd   Null-terminated command string
    */
    /**************************************************************************/
    void _process_cmd(char *cmd) {
        String str = String(cmd);

        // --- AZ - Set azimuth setpoint ---
        // Format: AZxxx.x
        if (str.startsWith("AZ")) {
            if (str.length() > 2) {
                control_az.setpoint = str.substring(2).toFloat();
                // Clamp to valid range
                control_az.setpoint = constrain(control_az.setpoint,
                                                MIN_M1_ANGLE, MAX_M1_ANGLE);
            } else {
                // Query current AZ position
                _send("AZ" + String(control_az.input, 1) + "\n");
            }
        }

        // --- EL - Set elevation setpoint ---
        // Format: ELxxx.x
        else if (str.startsWith("EL")) {
            if (str.length() > 2) {
                control_el.setpoint = str.substring(2).toFloat();
                control_el.setpoint = constrain(control_el.setpoint,
                                                MIN_M2_ANGLE, MAX_M2_ANGLE);
            } else {
                // Query current EL position
                _send("EL" + String(control_el.input, 1) + "\n");
            }
        }

        // --- SA - Stop azimuth ---
        else if (str.startsWith("SA")) {
            control_az.setpoint = control_az.input;
        }

        // --- SE - Stop elevation ---
        else if (str.startsWith("SE")) {
            control_el.setpoint = control_el.input;
        }

        // --- GS - Get rotator status ---
        // Response: GSx\n  (1=idle, 2=moving, 4=pointing, 8=error)
        else if (str.startsWith("GS")) {
            _send("GS" + String(rotator.rotator_status, DEC) + "\n");
        }

        // --- GE - Get rotator error ---
        // Response: GEx\n
        else if (str.startsWith("GE")) {
            _send("GE" + String(rotator.rotator_error, DEC) + "\n");
        }

        // --- VE - Get firmware version ---
        else if (str.startsWith("VE")) {
            _send("VE001\n");
        }

        // --- IP - Get current position (AZ and EL) ---
        // Some clients use this combined query
        else if (str.startsWith("IP")) {
            _send("AZ" + String(control_az.input, 1) +
                  " EL" + String(control_el.input, 1) + "\n");
        }

        // --- RB - Reboot ---
        else if (str.startsWith("RB")) {
            Serial.println("[INFO] Reboot requested");
            delay(100);
            ESP.restart();
        }

        // --- Unknown command - ignore silently ---
    }
};

#endif /* EASYCOMM_H_ */
