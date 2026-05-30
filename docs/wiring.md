# Wiring

Pin assignments live in `firmware/src/config.h`.

| Signal | XIAO label | Fallback GPIO | Notes |
|---|---:|---:|---|
| Left motor PWM | D2 | GPIO4 | Verify before wiring |
| Right motor PWM | D3 | GPIO5 | Verify before wiring |
| Battery ADC | D0 | GPIO2 | Placeholder only; do not connect LiPo directly |

## Low-Side MOSFET Concept

For each ERM motor:

```text
Battery + / motor supply +  -> motor +
motor -                    -> MOSFET drain
MOSFET source              -> GND
ESP32 PWM pin              -> gate through small resistor
GND                        -> shared ESP32 + battery + motor ground
```

Use a logic-level N-MOSFET that fully turns on at 3.3 V gate drive. Add motor suppression/protection appropriate to the motor and driver. Keep motor current out of the ESP32 pins.

## DRV8833 Option

The DRV8833 is heavier than two MOSFETs but easier to protect. Use one input per motor for one-direction PWM and tie the other input low, or follow your breakout board's recommended one-direction motor wiring.

## Battery Warning

Do not connect a LiPo cell directly to an ADC pin. Battery monitoring needs a resistor divider sized for the ESP32-C3 ADC input range and sleep current constraints.

