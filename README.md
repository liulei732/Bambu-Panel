# Bambu Panel

Bambu Panel is an ESP32-based 800x480 touch control screen for Bambu Lab printers.

The first target is a **Bambu Lab P1S** connected over the local network, with **two AMS units** and a dark, Bambu-like control-console UI. The goal is to build a machine-side primary control panel that can show print status, expose common controls, and stay useful even when optional data such as thumbnails or camera streaming is unavailable.

## Project Status

This project is in the design and bring-up stage.

Current focus:

- Define the 800x480 UI structure.
- Document the P1S local-control behavior needed by the screen.
- Prepare the ESP32 firmware project layout.
- Encode the first ESP32-S3 HX8369 + GT911 board profile and driver configuration layer.

## Target Hardware

- ESP32 development board.
- 800x480 landscape display.
- Capacitive touch.
- Wi-Fi LAN connection to the printer.
- Bambu Lab P1S.
- Two AMS units, eight filament slots total.

## UI Direction

The UI is designed as a primary control screen, not just a status monitor.

Main navigation:

- Home
- Files
- Control
- AMS
- Maintenance
- Settings

Home screen priorities:

- Current print state.
- File name, progress, and remaining time.
- Nozzle and bed temperature summary.
- Eight AMS slots.
- Quick actions: pause/resume, speed, fan, light, stop.

The default visual style is a dark control-console theme with cyan/teal accents and restrained red danger states.

## Repository Layout

```text
Bambu-Panel/
  assets/
    screenshots/      UI screenshots and visual references.
    ui/               UI images, icons, fonts, and generated assets.
  docs/
    design/           Product and UI design documents.
    hardware/         Wiring, board notes, display notes, enclosure notes.
    protocol/         Bambu LAN, MQTT, command, and state notes.
  firmware/
    esp32/            ESP32 firmware project.
  tools/              Local helper scripts and experiments.
```

## Documents

- [UI design spec](docs/design/p1s-esp32-control-screen-ui.md)
- [Software system architecture](docs/design/software-system-architecture.md)
- [UI design board](docs/design/mockups/bambu-panel-ui-board.html)
- [Interactive UI prototype](docs/design/mockups/bambu-panel-interactive-prototype.html)
- [UI board screenshot](assets/screenshots/bambu-panel-ui-board.png)
- [Interactive prototype screenshot](assets/screenshots/bambu-panel-interactive-prototype.png)
- [ESP32-S3 HX8369 + GT911 hardware profile](docs/hardware/esp32-s3-hx8369-gt911.md)

## Version 1 Scope

Version 1 focuses on a reliable local control experience:

- LAN connection setup.
- Home screen.
- Control page for nozzle, bed, fan, speed, flow, and preheat presets.
- AMS page for two AMS units.
- Maintenance page for low-frequency machine actions.
- Settings page for printer connection and screen configuration.

Not planned for the first version:

- Full SD card or cached-file browsing.
- Starting arbitrary files from the screen.
- Mandatory live camera on the home page.
- Cloud-dependent workflows.
- Multi-printer dashboard.

## Development Notes

The firmware should keep UI state, printer state, and network communication separated. The home screen should still work when optional data such as thumbnails, recent files, or camera frames are missing.

Connection failures and command failures should always surface as explicit UI states instead of silent no-ops.

## Verification

Run the host-side board profile test:
This also checks the I80 LCD config and GT911 coordinate transform.

```sh
tools/test-board-profiles.sh
```

Build the ESP32 firmware:

```sh
cd firmware/esp32
. /Users/liulei/esp/esp-idf/export.sh
idf.py -B build build
```
