# F4DIW Rotator

> ⚠️ **WORK IN PROGRESS — End-stop homing not yet tested.**

> Fork of [Satnogs-CNC-Rotator](https://github.com/jharwinbarrozo/Satnogs-CNC-Rotator)
> by [@jharwinbarrozo](https://github.com/jharwinbarrozo), adapted for **Wemos D1 R32 (ESP32) + Arduino CNC Shield v3**.

## What's different from upstream

| Feature | Upstream | This fork |
|---|---|---|
| MCU | Arduino Mega / SatNOGS board | Wemos D1 R32 (ESP32-WROOM) |
| Communication | RS485 / USB Serial | USB Serial + **Bluetooth SPP** |
| Compatible software | Gpredict | Gpredict + PST Rotator + **Look4Sat** |
| Pin mapping | SatNOGS board | CNC Shield v3 |
| ESP32 WDT | N/A | yield() in blocking loops |
| Park function | N/A | Auto-park AZ0/EL0 on BT disconnect |
| Jog commands | N/A | ML/MR/MU/MD + RST calibration |

## Hardware mod required

Cut resistor **R5** (10K) on CNC Shield v3 — pulls GPIO12 HIGH at boot.
See: https://onstep.groups.io/g/main/wiki/19670

Flash with shield **dismounted**.

## CNC Shield pin mapping

| Function | ESP32 GPIO |
|---|---|
| Step AZ (X-STEP) | GPIO 26 |
| Dir AZ (X-DIR) | GPIO 16 |
| Step EL (Y-STEP) | GPIO 25 |
| Dir EL (Y-DIR) | GPIO 27 |
| Motor Enable | GPIO 12 |
| End-stop AZ | GPIO 5 |
| End-stop EL | GPIO 23 |

## Supported commands

### EasyComm I + II

| Command | Description |
|---|---|
| AZxxx.x ELxxx.x | Set AZ and EL (EasyComm I) |
| AZxxx.x | Set azimuth |
| ELxxx.x | Set elevation |
| AZ | Query azimuth |
| EL | Query elevation |
| SA | Stop azimuth |
| SE | Stop elevation |
| GS | Get status |
| GE | Get error |
| IP | Get AZ+EL combined |
| VE | Get firmware version |

### Jog and Calibration

| Command | Description |
|---|---|
| ML | AZ - 1 degree (no limit) |
| MR | AZ + 1 degree (no limit) |
| MU | EL + 1 degree (no limit) |
| MD | EL - 1 degree (no limit) |
| RST | Reset counters to zero without moving |
| PK | Manual park AZ0 EL0 |
| RB | Reboot ESP32 |

### Calibration workflow
1. Send MD x n to move below physical zero
2. Send RST to set zero at current position
3. Rotator is calibrated

### Look4Sat (Android) Bluetooth
- Pair with **F4DIW-Rotator**
- Settings > Bluetooth data output
- Device id: BT MAC shown at boot
- Data format: P $AZ $EL

### PST Rotator / Gpredict USB Serial
- EasyComm I or II
- Baud rate: 9600

## Key parameters

| Parameter | Value | Description |
|---|---|---|
| RATIO | 19.2 | Gear reduction ratio |
| MICROSTEP | 8 | MS1+MS2 jumpers ON |
| SPR | 1600 | Steps per revolution |
| JOG_STEP | 1.0 | Jog step in degrees |
| SKIP_HOMING | 1 | Set to 0 with end-stops |

## Build

- Board: ESP32 Dev Module
- Partition: Default (not No BT)
- Upload speed: 921600

## Status

- [x] Compilation tested
- [x] Bluetooth pairing tested
- [x] Look4Sat BT tracking tested
- [x] Motor movement tested AZ + EL
- [x] PST Rotator USB Serial tested
- [x] Auto-park on BT disconnect
- [x] Jog ML/MR/MU/MD + RST
- [ ] End-stop homing (SKIP_HOMING=0)

## License

GPLv3

## Credits

- Original: [jharwinbarrozo/Satnogs-CNC-Rotator](https://github.com/jharwinbarrozo/Satnogs-CNC-Rotator)
- BT reference: [F4HTB/esp32Rotor](https://github.com/F4HTB/esp32Rotor)
- Pin mapping: [OnStep CNC3](https://github.com/hjd1964/OnStep)
- Adapted by: F4DIW
