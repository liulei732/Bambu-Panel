#ifndef BAMBU_LVGL_CONTROL_MODEL_H
#define BAMBU_LVGL_CONTROL_MODEL_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    int16_t nozzle_target_c;
    int16_t bed_target_c;
    int16_t fan_percent;
    int16_t speed_percent;
    bool light_on;
} bambu_lvgl_control_state_t;

typedef enum {
    BAMBU_LVGL_CONTROL_NOZZLE_DOWN = 0,
    BAMBU_LVGL_CONTROL_NOZZLE_UP,
    BAMBU_LVGL_CONTROL_BED_DOWN,
    BAMBU_LVGL_CONTROL_BED_UP,
    BAMBU_LVGL_CONTROL_FAN_DOWN,
    BAMBU_LVGL_CONTROL_FAN_UP,
    BAMBU_LVGL_CONTROL_SPEED_DOWN,
    BAMBU_LVGL_CONTROL_SPEED_UP,
    BAMBU_LVGL_CONTROL_LIGHT_TOGGLE,
} bambu_lvgl_control_action_t;

bambu_lvgl_control_state_t bambu_lvgl_control_state_default(void);
void bambu_lvgl_control_apply(bambu_lvgl_control_state_t *state, bambu_lvgl_control_action_t action);

#endif
