#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUT_DIR="${TMPDIR:-/tmp}"
PROFILE_OUT="${OUT_DIR}/bambu_panel_profiles_test"
DRIVER_OUT="${OUT_DIR}/bambu_panel_driver_configs_test"
UI_OUT="${OUT_DIR}/bambu_panel_home_scene_test"

if awk '
    /while \(true\)/ { in_loop = 1 }
    in_loop && /bambu_panel_ui_draw_home/ { found = 1 }
    END { exit found ? 0 : 1 }
' "${ROOT_DIR}/firmware/esp32/main/app_main.c"; then
  echo "app_main must not redraw the full home screen inside the touch loop" >&2
  exit 1
fi

if ! grep -q '^CONFIG_ESP_MAIN_TASK_STACK_SIZE=8192$' "${ROOT_DIR}/firmware/esp32/sdkconfig.defaults"; then
  echo "sdkconfig.defaults must reserve an 8192 byte main task stack for LCD/UI bring-up" >&2
  exit 1
fi

cc -std=c11 -Wall -Wextra -Werror \
  -I"${ROOT_DIR}/firmware/esp32/components/board_support/include" \
  "${ROOT_DIR}/tests/board_support/test_profiles.c" \
  "${ROOT_DIR}/firmware/esp32/components/board_support/board_profile.c" \
  "${ROOT_DIR}/firmware/esp32/components/board_support/display_profile.c" \
  -o "${PROFILE_OUT}"

"${PROFILE_OUT}"

cc -std=c11 -Wall -Wextra -Werror \
  -I"${ROOT_DIR}/firmware/esp32/components/board_support/include" \
  "${ROOT_DIR}/tests/board_support/test_driver_configs.c" \
  "${ROOT_DIR}/firmware/esp32/components/board_support/board_profile.c" \
  "${ROOT_DIR}/firmware/esp32/components/board_support/display_driver.c" \
  "${ROOT_DIR}/firmware/esp32/components/board_support/touch_driver.c" \
  -o "${DRIVER_OUT}"

"${DRIVER_OUT}"

cc -std=c11 -Wall -Wextra -Werror \
  -I"${ROOT_DIR}/firmware/esp32/components/panel_ui/include" \
  "${ROOT_DIR}/tests/panel_ui/test_home_scene.c" \
  "${ROOT_DIR}/firmware/esp32/components/panel_ui/bambu_panel_ui_scene.c" \
  -o "${UI_OUT}"

"${UI_OUT}"
