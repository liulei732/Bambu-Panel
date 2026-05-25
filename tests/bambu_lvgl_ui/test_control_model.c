#include <assert.h>
#include <stdbool.h>

#include "bambu_lvgl_control_model.h"

int main(void)
{
    bambu_lvgl_control_state_t state = bambu_lvgl_control_state_default();

    assert(state.nozzle_target_c == 220);
    assert(state.bed_target_c == 55);
    assert(state.fan_percent == 70);
    assert(state.speed_percent == 100);
    assert(!state.light_on);

    bambu_lvgl_control_apply(&state, BAMBU_LVGL_CONTROL_NOZZLE_UP);
    assert(state.nozzle_target_c == 225);
    bambu_lvgl_control_apply(&state, BAMBU_LVGL_CONTROL_NOZZLE_DOWN);
    assert(state.nozzle_target_c == 220);

    bambu_lvgl_control_apply(&state, BAMBU_LVGL_CONTROL_BED_UP);
    assert(state.bed_target_c == 60);
    bambu_lvgl_control_apply(&state, BAMBU_LVGL_CONTROL_BED_DOWN);
    assert(state.bed_target_c == 55);

    bambu_lvgl_control_apply(&state, BAMBU_LVGL_CONTROL_FAN_UP);
    assert(state.fan_percent == 80);
    bambu_lvgl_control_apply(&state, BAMBU_LVGL_CONTROL_FAN_DOWN);
    assert(state.fan_percent == 70);

    bambu_lvgl_control_apply(&state, BAMBU_LVGL_CONTROL_SPEED_UP);
    assert(state.speed_percent == 105);
    bambu_lvgl_control_apply(&state, BAMBU_LVGL_CONTROL_SPEED_DOWN);
    assert(state.speed_percent == 100);

    bambu_lvgl_control_apply(&state, BAMBU_LVGL_CONTROL_LIGHT_TOGGLE);
    assert(state.light_on);
    bambu_lvgl_control_apply(&state, BAMBU_LVGL_CONTROL_LIGHT_TOGGLE);
    assert(!state.light_on);

    state.nozzle_target_c = 300;
    bambu_lvgl_control_apply(&state, BAMBU_LVGL_CONTROL_NOZZLE_UP);
    assert(state.nozzle_target_c == 300);

    state.bed_target_c = 0;
    bambu_lvgl_control_apply(&state, BAMBU_LVGL_CONTROL_BED_DOWN);
    assert(state.bed_target_c == 0);

    state.fan_percent = 100;
    bambu_lvgl_control_apply(&state, BAMBU_LVGL_CONTROL_FAN_UP);
    assert(state.fan_percent == 100);

    state.speed_percent = 50;
    bambu_lvgl_control_apply(&state, BAMBU_LVGL_CONTROL_SPEED_DOWN);
    assert(state.speed_percent == 50);

    return 0;
}
