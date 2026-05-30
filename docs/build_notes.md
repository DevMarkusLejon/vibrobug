# Build Notes

## PlatformIO

Build:

```bash
pio run
```

Upload:

```bash
pio run --target upload
```

Monitor:

```bash
pio device monitor
```

## Board Target

The requested target is `seeed_xiao_esp32c3`. It was verified with PlatformIO 6.1.19:

```text
ID                  MCU      Frequency    Flash    RAM    Name
seeed_xiao_esp32c3  ESP32C3  160MHz       4MB      320KB  Seeed Studio XIAO ESP32C3
```

If needed:

```bash
pio boards seeed
pio boards esp32c3
```

Then update `platformio.ini` and re-run `pio run`.

## Expected Serial Output

On boot, the firmware prints the AP SSID and IP address. Drive commands also print the received mode and motor targets.
