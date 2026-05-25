# LVGL Home Page Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Replace the temporary rectangle/text renderer with a first LVGL-rendered home page that visibly matches the 800x480 UI mockup much more closely.

**Architecture:** Keep the working HX8369 I80 display and GT911 touch drivers. Add a thin LVGL port component that registers display flush, touch input, and tick handling. Add a separate LVGL UI component for theme tokens and the home page so later pages can reuse the same styles.

**Tech Stack:** ESP-IDF 5.4, local LVGL v8-style component from `/Users/liulei/esp32/LVGL_4.3inch_GT911`, HX8369 I80 panel driver, GT911 touch driver.

---

### Task 1: Vendor A Minimal LVGL Component

**Files:**
- Create: `firmware/esp32/components/lvgl/CMakeLists.txt`
- Copy: `firmware/esp32/components/lvgl/src/**`
- Copy: `firmware/esp32/components/lvgl/lvgl.h`
- Copy: `firmware/esp32/components/lvgl/lv_conf.h`

- [ ] Copy only LVGL runtime sources, not docs, tests, demos, or examples.
- [ ] Use a local `CMakeLists.txt` that builds `src/*.c` recursively and exposes `.` and `src` include directories.
- [ ] Keep `LV_COLOR_DEPTH` at 16 and enable complex software drawing for radius and masks.
- [ ] Run `idf.py -B build build`; expected result is compilation reaching app code or LVGL API errors, not missing `lvgl.h`.

### Task 2: Add Pixel-Buffer Flush Support To Panel Driver

**Files:**
- Modify: `firmware/esp32/components/panel_hw/include/bambu_panel_hw.h`
- Modify: `firmware/esp32/components/panel_hw/bambu_panel_hw.c`
- Test: `tools/test-board-profiles.sh`

- [ ] Add `bambu_panel_hw_draw_rgb565_bitmap(panel, x, y, w, h, pixels)`.
- [ ] Send each LVGL flush as small independent RAMWR windows, using the same chunking discipline that removed diagonal and horizontal artifacts.
- [ ] Keep `bambu_panel_hw_fill_rect_rgb565` as a color-fill wrapper for tests and simple fallback screens.
- [ ] Add a host test assertion that the new public symbol is declared by including the header in the existing test build.
- [ ] Run `tools/test-board-profiles.sh`; expected result is pass.

### Task 3: Add LVGL Port Component

**Files:**
- Create: `firmware/esp32/components/bambu_lvgl_port/CMakeLists.txt`
- Create: `firmware/esp32/components/bambu_lvgl_port/include/bambu_lvgl_port.h`
- Create: `firmware/esp32/components/bambu_lvgl_port/bambu_lvgl_port.c`
- Modify: `firmware/esp32/main/CMakeLists.txt`

- [ ] Initialize LVGL with two DMA-capable draw buffers.
- [ ] Register an LVGL display driver with `hor_res=800`, `ver_res=480`, and a flush callback that calls `bambu_panel_hw_draw_rgb565_bitmap`.
- [ ] Register a pointer input driver that reads `bambu_touch_hw_read`.
- [ ] Start a periodic `esp_timer` that calls `lv_tick_inc(2)`.
- [ ] Expose `bambu_lvgl_port_init`, `bambu_lvgl_port_lock`, `bambu_lvgl_port_unlock`, and `bambu_lvgl_port_run`.
- [ ] Build with `idf.py -B build build`; expected result is pass.

### Task 4: Build LVGL Theme And Home Page

**Files:**
- Create: `firmware/esp32/components/bambu_lvgl_ui/CMakeLists.txt`
- Create: `firmware/esp32/components/bambu_lvgl_ui/include/bambu_lvgl_ui.h`
- Create: `firmware/esp32/components/bambu_lvgl_ui/bambu_lvgl_ui.c`
- Modify: `firmware/esp32/main/CMakeLists.txt`

- [ ] Create a dark theme with 8 px card radius, restrained borders, cyan active state, amber warning state, and red danger state.
- [ ] Build the home page with top bar, 6-item left nav, job preview card, temperature cards, AMS slot summary, and 5 bottom action buttons.
- [ ] Use English labels for the first firmware milestone if Chinese font assets are not yet available; keep layout ready for Chinese fonts later.
- [ ] Match the mockup geometry first: top bar 52 px, rail 88 px, content starting at x=88, job card left, status stack right, actions bottom.
- [ ] Build with `idf.py -B build build`; expected result is pass.

### Task 5: Switch App Main To LVGL Runtime

**Files:**
- Modify: `firmware/esp32/main/app_main.c`
- Test: `tools/test-board-profiles.sh`

- [ ] Keep board/display/touch logging.
- [ ] Initialize `bambu_panel_hw` and `bambu_touch_hw` exactly as before.
- [ ] Replace direct `bambu_panel_ui_draw_page` startup drawing with `bambu_lvgl_port_init` and `bambu_lvgl_ui_show_home`.
- [ ] Replace the old polling UI loop with `bambu_lvgl_port_run`, leaving printer protocol work for a later stage.
- [ ] Run `tools/test-board-profiles.sh`; expected result is pass.
- [ ] Run `idf.py -B build build`; expected result is pass.
- [ ] Flash and monitor `/dev/cu.usbserial-1120`; expected result is boot log, GT911 ready, LVGL home page visible, and no continuous full-screen flashing.

### Task 6: Commit The Stage

**Files:**
- Stage all created and modified LVGL migration files.

- [ ] Run `git diff --check`; expected result is no output.
- [ ] Run `tools/test-board-profiles.sh`; expected result is pass.
- [ ] Run `idf.py -B build build`; expected result is pass.
- [ ] Commit with message `Add LVGL home page prototype`.
