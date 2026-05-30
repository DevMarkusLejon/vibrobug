# Electrokit Quickorder Notes

The quickorder CSV is `docs/electrokit_quickorder.csv`.

Electrokit's quick order page says each line should contain SKU first, then quantity, separated by semicolon, comma, or space. The CSV therefore uses:

```text
sku;qty
```

with no header row.

## Included Items

| SKU | Qty | Item |
|---:|---:|---|
| 41024000 | 1 | Seeed Studio XIAO ESP32-C3 |
| 41032757 | 6 | 3.3 V vibration motor |
| 41018597 | 10 | AO3400A N-channel MOSFET |
| 41017708 | 4 | SOT-23 / SOT-363 adapter board |
| 40810210 | 10 | 100 ohm resistor |
| 40810510 | 10 | 100k resistor |
| 41013389 | 8 | 1N5819 Schottky diode |
| 41017679 | 2 | 100 uF 16 V electrolytic capacitor |
| 41015673 | 4 | 100 nF capacitor |
| 41017310 | 2 | 3.7 V 150 mAh LiPo battery |
| 41011707 | 1 | MicroLipo USB LiPo charger |
| 41024383 | 1 | Slide power switch |
| 41013877 | 1 | 2-pin JST-PH cable/header |
| 41012199 | 1 | 400-point breadboard |
| 41001167 | 1 | 2.54 mm male header strip |
| 41014959 | 1 | AWG22 solid-core wire assortment |

## Not Included

- Heat-shrink assortment `41022259` was omitted because the product page showed it as out of stock.
- The TB6612FNG dual motor driver option was omitted because this cart follows the MOSFET low-side driver approach used by the current docs.
- Mechanical experiment items such as toothbrushes, tape, chassis material, screws, TPU, or silicone feet should be bought separately as needed.

