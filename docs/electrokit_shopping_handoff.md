# Electrokit Shopping Handoff

Goal: put together an Electrokit shopping cart for the first `vibrobug` prototype: a Seeed XIAO ESP32-C3 two-motor vibration robot with MOSFET motor drivers, small LiPo power, and basic prototyping supplies.

Prefer Electrokit items where available. If an exact linked item is unavailable, choose the closest equivalent with the same electrical role and note the substitution.

## Core Cart

| Qty | Item | Electrokit link | Notes |
|---:|---|---|---|
| 1 | Seeed Studio XIAO ESP32-C3 | https://www.electrokit.com/seeed-studio-xiao-esp32c3 | Required controller; firmware targets this board. |
| 4-6 | 3.3 V vibration motor | https://www.electrokit.com/en/product.php/25486 | Use 2 on robot; extras for experiments/damage. |
| 10 | AO3400A N-channel MOSFET, SOT-23 | https://www.electrokit.com/ao3400a-sot-23-n-ch-30v-5.7a | Low-side PWM motor switches. |
| 4 | SOT-23 to DIP adapter boards | https://www.electrokit.com/en/adapterkort-sot23-dip | Makes MOSFET prototyping easier. |
| 10 | 100 ohm resistors, 0.25 W | https://www.electrokit.com/en/motstand-kolfilm-0.25w-100ohm-100r | Gate resistors. |
| 10 | 100k resistors, 0.25 W | https://www.electrokit.com/en/motstand-kolfilm-0.25w-100kohm-100k | Gate pulldowns. |
| 4 | 1N5819 Schottky diodes | Search Electrokit for `1N5819` | Motor flyback/protection. |
| 2 | 100 uF electrolytic capacitor, 16 V or higher | https://www.electrokit.com/en/el.lyt-100uf-16v-105c-6.3x5mm | Motor supply smoothing. |
| 4 | 100 nF capacitor | https://www.electrokit.com/en/r82-100nf-100v-55mm | Local noise suppression. |
| 1-2 | LiPo battery, 3.7 V, about 150 mAh, JST-PH | https://www.electrokit.com/en/batteri-lipo-3.7v-150mah | Small/light prototype battery. |
| 1 | MicroLipo USB charger with 100 mA charge option | https://www.electrokit.com/en/microlipo-usb-laddare | Use 100 mA for 150 mAh LiPo. |
| 1 | Slide power switch, PCB, 2.54 mm | https://www.electrokit.com/en/skjutomkopplare-1-pol-on-on-pcb-2.54mm-bla | Physical cutoff. |
| 1 | JST-PH 2-pin cable, 150 mm | https://www.electrokit.com/kabel-med-vinklad-kontakt-jst-ph-2-pol-150mm | Battery/prototype wiring. |
| 1 | Breadboard, 400 points | https://www.electrokit.com/kopplingsdack-400-anslutningar | Bench testing before soldering. |
| 1 | 2.54 mm male header strip | Search Electrokit for `stiftlist 2.54mm` | Headers for XIAO/adapters if needed. |
| 1 | Hook-up wire assortment | https://www.electrokit.com/en/hook-up-wire/ | Use flexible wire for prototype wiring. |
| 1 | Heat-shrink assortment | https://www.electrokit.com/en/krympslangset-blandade-farger-med-87-langder | Insulation and strain relief. |

## Easier Motor Driver Alternative

If the buyer wants less tiny SOT-23 soldering, add this instead of the MOSFET/adapters/diodes path:

| Qty | Item | Electrokit link | Notes |
|---:|---|---|---|
| 1 | Dual TB6612FNG motor driver with mounted headers | https://www.electrokit.com/en/motordrivare-1a-dual-tb6612fng-stiftlist-monterad | Easier prototype wiring, but larger/heavier than MOSFETs. |

If using the TB6612FNG option, still keep resistors/capacitors useful for general prototyping, but the MOSFET-specific adapter boards and Schottky flyback diodes are less critical.

## Mechanical Items To Buy Elsewhere

Electrokit may not be the best source for these:

- Toothbrush heads or toothbrushes for angled bristles.
- Double-sided foam tape.
- Thin plastic sheet, small plastic shell, or 3D printed chassis.
- TPU/silicone tabs or fine spring wire for later leg experiments.
- Small screws, nuts, and standoffs if using a printed chassis.

## Safety Notes For Cart Agent

- The LiPo battery must not be charged above its safe charge rate. For a 150 mAh cell, use the charger at 100 mA.
- Do not connect a LiPo directly to the ESP32 ADC pin. Battery measurement needs a resistor divider added later.
- The first build is not cat-safe. It needs a fully enclosed shell, no loose bristles/wires, no exposed solder joints, and a physical power switch before any animal use.

## If Items Are Out Of Stock

Acceptable substitutions:

- XIAO ESP32-C3: prefer exact match. If unavailable, stop and ask before substituting the controller.
- Vibration motors: any small 3 V to 3.3 V ERM vibration motor is acceptable.
- MOSFET: any logic-level N-MOSFET that turns on well at 3.3 V and can handle motor current is acceptable.
- Diode: any small Schottky diode suitable for motor flyback current is acceptable.
- LiPo: 100-300 mAh is preferred for the tiny prototype; larger cells are easier to charge safely but add weight.
- Charger: must support a charge current appropriate for the selected LiPo.

