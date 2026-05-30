# VibroBug

VibroBug is a small steerable vibration-powered insect robot. The MVP uses two independent vibration motors so the left and right side can be driven at different intensities from a phone browser.

This is an engineering prototype. Do not use it with an animal until the electronics, battery, wires, bristles, solder joints, and sharp edges are fully enclosed and durability-tested.

## Hardware

Minimum prototype bill of materials:

| Item | Quantity | Notes |
|---|---:|---|
| Seeed Studio XIAO ESP32-C3 | 1 | Wi-Fi AP controller |
| 3 V ERM vibration motors | 2-6 | Two installed, extras for experiments |
| Logic-level N-MOSFETs or DRV8833 breakout | 2 MOSFETs or 1 dual driver | MOSFETs are light; DRV8833 is more protected |
| 1S LiPo, 100-300 mAh | 1-2 | Keep small and protected |
| LiPo charger/protection board | 1 | Use safe charging practices |
| Physical power switch | 1 | Hard cutoff |
| Toothbrush bristles, TPU, wire, or silicone legs | assorted | Locomotion experiments |
| Heat shrink, tape, screws, small shell | assorted | Prototype assembly |

## Wiring

Initial firmware pin assumptions are centralized in [config.h](</C:/Users/User/scuttlebug/firmware/src/config.h>):

| Function | XIAO label | Fallback GPIO |
|---|---:|---:|
| Left motor PWM | D2 | GPIO4 |
| Right motor PWM | D3 | GPIO5 |
| Battery ADC placeholder | D0 | GPIO2 |

Verify these labels against your installed PlatformIO board package and your exact Seeed XIAO ESP32-C3 pinout before connecting hardware.

For each low-side MOSFET motor driver:

| Connection | Destination |
|---|---|
| Battery/motor supply + | Motor + |
| Motor - | MOSFET drain |
| MOSFET source | Shared GND |
| ESP32 PWM pin | Gate through a small resistor |
| ESP32 GND, battery GND, motor supply GND | Common ground |

Add suitable flyback/protection for inductive motors. If using a DRV8833 breakout, follow that board's wiring guide instead.

## Firmware

Install PlatformIO, then build:

```bash
pio run
```

Flash and monitor:

```bash
pio run --target upload
pio device monitor
```

The project currently targets:

```ini
[env:seeed_xiao_esp32c3]
platform = espressif32
board = seeed_xiao_esp32c3
framework = arduino
```

If that board ID is unavailable in your PlatformIO install, run `pio boards seeed` or `pio boards esp32c3`, choose the closest XIAO ESP32-C3 target, and update `platformio.ini`.

## Phone Control

After boot, the ESP32 creates a Wi-Fi network named like:

```text
VibroBug-1234
```

Connect your phone or laptop to that network and open:

```text
http://192.168.4.1/
```

The web UI sends repeated drive commands while the joystick is held. Releasing touch sends `/stop`. Firmware also stops the motors if commands go stale for `COMMAND_TIMEOUT_MS`.

## API

Drive with throttle and turn:

```bash
curl -X POST http://192.168.4.1/drive \
  -H "Content-Type: application/json" \
  -d '{"mode":"drive","throttle":0.7,"turn":0.3}'
```

Direct motor tuning:

```bash
curl -X POST http://192.168.4.1/drive \
  -H "Content-Type: application/json" \
  -d '{"mode":"direct","left":0.65,"right":0.30}'
```

Start a movement pattern:

```bash
curl -X POST http://192.168.4.1/drive \
  -H "Content-Type: application/json" \
  -d '{"mode":"pattern","name":"wiggle","intensity":0.7,"duration_ms":3000}'
```

Stop immediately:

```bash
curl -X POST http://192.168.4.1/stop
```

Read status:

```bash
curl http://192.168.4.1/status
```

Update runtime config:

```bash
curl -X POST http://192.168.4.1/config \
  -H "Content-Type: application/json" \
  -d '{"max_duty":0.7,"left_trim":1.0,"right_trim":0.9}'
```

## Safety Defaults

| Setting | Value |
|---|---:|
| Command timeout | 500 ms |
| Max duty | 0.80 |
| Ramp rate | 2.0 per second |
| Boot state | motors off |
| Fail state | motors off |

## Tuning Checklist

1. Run one motor at a time and confirm vibration plus angled legs creates motion.
2. Test direct mode: left high/right low, right high/left low, both equal.
3. Adjust foot angle, motor position, and battery placement before changing firmware.
4. Use max duty and trim sliders to avoid excessive current and balance turning.
5. Record short videos and notes for every mechanical configuration.

