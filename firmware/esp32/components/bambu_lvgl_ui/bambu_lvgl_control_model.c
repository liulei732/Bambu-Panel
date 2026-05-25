#include "bambu_lvgl_control_model.h"

static int16_t clamp_i16(int16_t value, int16_t min, int16_t max)
{
    if (value < min) {
        return min;
    }
    if (value > max) {
        return max;
    }
    return value;
}

bambu_lvgl_control_state_t bambu_lvgl_control_state_default(void)
{
    return (bambu_lvgl_control_state_t) {
        .nozzle_target_c = 220,
        .bed_target_c = 55,
        .fan_percent = 70,
        .speed_percent = 100,
        .light_on = false,
    };
}

void bambu_lvgl_control_apply(bambu_lvgl_control_state_t *state, bambu_lvgl_control_action_t action)
{
    if (state == 0) {
        return;
    }

    switch (action) {
    case BAMBU_LVGL_CONTROL_NOZZLE_DOWN:
        state->nozzle_target_c = clamp_i16(state->nozzle_target_c - 5, 0, 300);
        break;
    case BAMBU_LVGL_CONTROL_NOZZLE_UP:
        state->nozzle_target_c = clamp_i16(state->nozzle_target_c + 5, 0, 300);
        break;
    case BAMBU_LVGL_CONTROL_BED_DOWN:
        state->bed_target_c = clamp_i16(state->bed_target_c - 5, 0, 120);
        break;
    case BAMBU_LVGL_CONTROL_BED_UP:
        state->bed_target_c = clamp_i16(state->bed_target_c + 5, 0, 120);
        break;
    case BAMBU_LVGL_CONTROL_FAN_DOWN:
        state->fan_percent = clamp_i16(state->fan_percent - 10, 0, 100);
        break;
    case BAMBU_LVGL_CONTROL_FAN_UP:
        state->fan_percent = clamp_i16(state->fan_percent + 10, 0, 100);
        break;
    case BAMBU_LVGL_CONTROL_SPEED_DOWN:
        state->speed_percent = clamp_i16(state->speed_percent - 5, 50, 200);
        break;
    case BAMBU_LVGL_CONTROL_SPEED_UP:
        state->speed_percent = clamp_i16(state->speed_percent + 5, 50, 200);
        break;
    case BAMBU_LVGL_CONTROL_LIGHT_TOGGLE:
        state->light_on = !state->light_on;
        break;
    }
}
