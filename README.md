# F4DIW Rotator

> ⚠️ **WORK IN PROGRESS — Partially tested. End-stop homing not yet tested.**

> Fork of [Satnogs-CNC-Rotator](https://github.com/jharwinbarrozo/Satnogs-CNC-Rotator)
> by [@jharwinbarrozo](https://github.com/jharwinbarrozo), adapted for **Wemos D1 R32 (ESP32) + Arduino CNC Shield v3**.

---

## What's different from upstream

| Feature | Upstream | This fork |
|---|---|---|
| MCU | Arduino Mega / SatNOGS board | Wemos D1 R32 (ESP32-WROOM) |
| Communication | RS485 / USB Serial | USB Serial + **Bluetooth SPP** |
| Compatible software | Gpredict | Gpredict + PST Rotator + **Look4Sat** |
| `easycomm.h` | Not included | Full EasyComm I+II + Look4Sat protocol |
| Pin mapping | SatNOGS board | CNC Shield v3 (`cnc_shield_rotator_pins.h`) |
| ESP32 WDT | N/A | `yield()` added in blocking loops |
| Global variables | Defined in header | `extern` in header, defined in `.ino` |
| Park function | N/A | Auto-park AZ0/EL0 on BT disconnect |

---

## Hardware

| Component | Model |
|---|---|
| MCU board | Wemos D1 R32 (ESP32-WROOM) |
| Shield | Arduino CNC Shield v3 |
| Stepper drivers | A4988 or DRV8825 |
| Bluetooth | ESP32 built-in (Classic SPP, no extra module needed) |
| End-stops | Mechanical, NO or NC (configurable) |

### ⚠️ Hardware mod required

The CNC Shield v3 has a **10K pull-up resistor (R5)** on the EN pin (GPIO12) that pulls it HIGH at boot, preventing the ESP32 from starting.

**You must cut or unsolder R5** before using this firmware with the shield mounted.
See: https://onstep.groups.io/g/main/wiki/19670

### CNC Shield pin mapping (Wemos D1 R32 / ESP32)

| Function | Arduino pin | ESP32 GPIO |
|---|---|---|
| Step AZ (X-STEP) | D2 | GPIO 26 |
| Dir AZ (X-DIR) | D5 | GPIO 16 |
| Step EL (Y-STEP) | D3 | GPIO 25 |
| Dir EL (Y-DIR) | D6 | GPIO 27 |
| Motor Enable | D8 | GPIO 12 |
| End-stop AZ (X-MIN) | D9 | GPIO 5 |
| End-stop EL (Y-MIN) | D10 | GPIO 23 |

Verified against [OnStep CNC3 pinmap](https://github.com/hjd1964/OnStep/blob/master/src/pinmaps/Pins.CNC3.h).

---

## Supported protocols

### Look4Sat (Android) — Bluetooth SPP
Look4Sat sends position data in format `P az el` without line endings between frames.
The firmware splits on the `P` delimiter to parse each frame correctly.

Setup in Look4Sat:
1. Pair Android with **`F4DIW-Rotator`** in Bluetooth settings
2. **Settings → Bluetooth data output**
   - Enable rotation output: **ON**
   - Device id: your ESP32 BT MAC (shown in Serial monitor at boot)
   - Data format: `P $AZ $EL` (default)

### PST Rotator / Gpredict — USB Serial
- Protocol: EasyComm I (`AZxxx.x ELxxx.x` on one line) or EasyComm II (separate commands)
- Baud rate: **9600**

### Supported EasyComm commands

| Command | Description |
|---|---|
| `AZxxx.x ELxxx.x` | Set AZ and EL (EasyComm I) |
| `AZxxx.x` | Set azimuth setpoint |
| `ELxxx.x` | Set elevation setpoint |
| `AZ` | Query current azimuth |
| `EL` | Query current elevation |
| `SA` | Stop azimuth |
| `SE` | Stop elevation |
| `GS` | Get rotator status |
| `GE` | Get rotator error code |
| `IP` | Get AZ+EL position combined |
| `VE` | Get firmware version |
| `PK` | Manual park (AZ0 EL0) |
| `RB` | Reboot ESP32 |

---

## Key parameters

| Parameter | Value | Description |
|---|---|---|
| `RATIO` | 19.2 | Mechanical gear reduction ratio |
| `MICROSTEP` | 8 | Driver microstep (MS1+MS2 jumpers ON) |
| `SPR` | 1600 | Steps per revolution (200 × 8) |
| `MAX_SPEED` | 3200 | Steps/s |
| `SKIP_HOMING` | 1 | Set to 0 when end-stops are connected |

---

## Build & Flash

- Board: **ESP32 Dev Module** (Arduino IDE)
- Partition scheme: **Default** (must include Bluetooth — NOT "No BT")
- Upload speed: `921600`
- Required libraries:
  - [AccelStepper](https://www.airspayce.com/mikem/arduino/AccelStepper/)
  - `BluetoothSerial` (included in ESP32 Arduino core)

### ⚠️ Flash with shield dismounted
The shield must be removed before flashing — GPIO12 (EN) conflicts with the ESP32 boot process even after cutting R5.

### Microstep jumpers (CNC Shield)

| Microstep | MS1 | MS2 | MS3 |
|---|---|---|---|
| 1/8 | ON | ON | OFF |
| 1/16 | ON | ON | ON |

Default: **1/8** (`MICROSTEP 8`)

---

## Serial monitor output at boot

```
[INFO] EasyComm ready - USB + BT
[INFO] BT Name: F4DIW-Rotator
[INFO] BT MAC:  10:52:1C:67:0C:BE
[INFO] Homing SKIPPED
```

---

## Status

- [x] Compilation tested ✅
- [x] Bluetooth pairing tested ✅
- [x] Look4Sat BT tracking tested ✅
- [x] Motor movement tested ✅ (AZ + EL bidirectional)
- [x] PST Rotator USB Serial tested ✅
- [x] Auto-park on BT disconnect ✅
- [ ] End-stop homing tested (SKIP_HOMING=0)

---

## License

GPLv3 — see [LICENSE](LICENSE)

---

## Credits

- Original project: [jharwinbarrozo/Satnogs-CNC-Rotator](https://github.com/jharwinbarrozo/Satnogs-CNC-Rotator)
- SatNOGS project: [satnogs.org](https://satnogs.org)
- BT callback reference: [F4HTB/esp32Rotor](https://github.com/F4HTB/esp32Rotor)
- Pin mapping reference: [OnStep CNC3](https://github.com/hjd1964/OnStep)
- Adapted by: **F4DIW**
