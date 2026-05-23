/*!
 * @file cnc_shield_rotator_pins.h
 * Pin mapping for Wemos D1 R32 (ESP32) + Arduino CNC Shield v3
 * Verified against OnStep CNC3 pinmap and hardware tests
 */
#ifndef ROTATOR_PINS_H_
#define ROTATOR_PINS_H_

#define M1IN1    26  ///< Step AZ  (X-STEP)
#define M1IN2    16  ///< Dir AZ   (X-DIR)
#define M2IN1    25  ///< Step EL  (Y-STEP)
#define M2IN2    27  ///< Dir EL   (Y-DIR)
#define MOTOR_EN 12  ///< Enable motors (active LOW)
#define SW1       5  ///< End-stop AZ (X-MIN)
#define SW2      23  ///< End-stop EL (Y-MIN)
#define RS485_DIR 4
#define SDA_PIN   21
#define SCL_PIN   22

#endif /* ROTATOR_PINS_H_ */
