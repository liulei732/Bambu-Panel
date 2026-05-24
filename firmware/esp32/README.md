# ESP32 Firmware

This directory will contain the ESP32 firmware project.

Planned stack:

- ESP-IDF.
- LVGL for the 800x480 touch UI.
- Wi-Fi LAN connection to the printer.
- Local state model for printer status, AMS status, and UI navigation.

Current code:

- `components/board_support` defines the first board profile for the ESP32-S3 4.3 inch HX8369 + GT911 hardware.
- `components/board_support` derives I80 LCD and GT911 touch driver configs from the board profile.
- `components/board_support` includes a host-tested GT911 coordinate transform for the current screen orientation.
- `components/panel_hw` initializes the HX8369 over the ESP-IDF I80 LCD API and fills a dark boot screen.
- `components/panel_ui` draws the first board-rendered home screen skeleton directly through the panel driver.
- `components/touch_hw` initializes GT911 over the ESP-IDF I2C master API and reads first-touch reports.
- `main/app_main.c` logs the selected board, display, LCD driver, and touch driver profiles at boot, then brings up the panel and touch hardware.

Reference hardware profile:

- Display: HX8369, Intel 8080 / I80 8-bit, 800x480, RGB565.
- Touch: GT911 over I2C.
- Stable LCD pixel clock: 30 MHz.
- Measured GT911 address on this board: `0x5d`, with `0x14` as fallback.
- Touch transform: swap X/Y, invert Y.

Host-side verification:

```sh
../../tools/test-board-profiles.sh
```
