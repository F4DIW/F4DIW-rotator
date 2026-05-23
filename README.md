# F4DIW Rotator

> ⚠️ **WORK IN PROGRESS — Partially tested. Use at your own risk.**

> Fork of [Satnogs-CNC-Rotator](https://github.com/jharwinbarrozo/Satnogs-CNC-Rotator)
> by [@jharwinbarrozo](https://github.com/jharwinbarrozo), adapted for **Wemos D1 R32 (ESP32) + Arduino CNC Shield v3**.

## What's different from upstream

| Feature | Upstream | This fork |
|---|---|---|
| MCU | Arduino Mega / SatNOGS board | Wemos D1 R32 (ESP32) |
| Communication | RS485 / USB Serial | USB Serial + **Bluetooth SPP** |
| Compatible software | Gpredict | Gpredict + **Look4Sat** |
| `easycomm.h` | Not included | Full EasyComm II implementation |
| Pin mapping | SatNOGS board | CNC Shield v3 (`cnc_shield_rotator_pins.h`) |
| ESP32 WDT | N/A | `yield()` added in blocking loops |

## Hardware

| Component | Model |
|---|---|
| MCU board | Wemos D1 R32 (ESP32-WROOM) |
| Shield | Arduino CNC Shield v3 |
| Stepper drivers | A4988 or DRV8825 |
| Bluetooth | ESP32 built-in (Classic SPP) |

## Build & Flash

- Board: **ESP32 Dev Module**
- Partition scheme: **Default** (not "No BT")
- Upload speed: `921600`

## Compatible software

- [Look4Sat](https://github.com/rt-bishop/Look4Sat) via Bluetooth SPP (`SatNOGS-Rotator`)
- [Gpredict](http://gpredict.oz9aec.net/) via USB Serial / rotcld (9600 baud)

## Status

- [x] Compilation tested ✅
- [x] Bluetooth pairing tested ✅
- [x] Look4Sat connection tested
- [x] Motor movement tested
- [ ] End-stop homing tested
- [ ] Full tracking tested

## License

GPLv3 — see [LICENSE](LICENSE)

## Credits

- Original project: [jharwinbarrozo/Satnogs-CNC-Rotator](https://github.com/jharwinbarrozo/Satnogs-CNC-Rotator)
- Adapted by: F4DIW
