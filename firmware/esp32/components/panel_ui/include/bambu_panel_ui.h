#ifndef BAMBU_PANEL_UI_H
#define BAMBU_PANEL_UI_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    BAMBU_PANEL_UI_CMD_RECT = 0,
    BAMBU_PANEL_UI_CMD_TEXT,
} bambu_panel_ui_command_type_t;

typedef enum {
    BAMBU_PANEL_UI_HIT_NONE = 0,
    BAMBU_PANEL_UI_HIT_PAUSE,
    BAMBU_PANEL_UI_HIT_LIGHT,
    BAMBU_PANEL_UI_HIT_FAN,
    BAMBU_PANEL_UI_HIT_STOP,
} bambu_panel_ui_hit_t;

typedef struct {
    bool paused;
    bool chamber_light_on;
    uint8_t part_fan_percent;
    bool stop_confirm_pending;
} bambu_panel_ui_state_t;

typedef enum {
    BAMBU_PANEL_UI_ACTION_NONE = 0,
    BAMBU_PANEL_UI_ACTION_PAUSE_PRINT,
    BAMBU_PANEL_UI_ACTION_RESUME_PRINT,
    BAMBU_PANEL_UI_ACTION_SET_CHAMBER_LIGHT,
    BAMBU_PANEL_UI_ACTION_SET_PART_FAN,
    BAMBU_PANEL_UI_ACTION_REQUEST_STOP_CONFIRMATION,
} bambu_panel_ui_action_type_t;

typedef struct {
    bambu_panel_ui_action_type_t type;
    int16_t value;
} bambu_panel_ui_action_t;

typedef enum {
    BAMBU_PANEL_UI_TOUCH_EVENT_NONE = 0,
    BAMBU_PANEL_UI_TOUCH_EVENT_PRESS,
    BAMBU_PANEL_UI_TOUCH_EVENT_RELEASE,
} bambu_panel_ui_touch_event_type_t;

typedef struct {
    bool pressed;
    uint8_t release_samples;
    uint16_t press_x;
    uint16_t press_y;
    bambu_panel_ui_hit_t press_hit;
} bambu_panel_ui_touch_tracker_t;

typedef struct {
    bambu_panel_ui_touch_event_type_t type;
    uint16_t x;
    uint16_t y;
    bambu_panel_ui_hit_t hit;
} bambu_panel_ui_touch_event_t;

typedef struct {
    bambu_panel_ui_command_type_t type;
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
    uint16_t color;
    uint8_t text_scale;
    char text[32];
} bambu_panel_ui_command_t;

typedef struct bambu_panel_hw_t bambu_panel_hw_t;

size_t bambu_panel_ui_home_scene(bambu_panel_ui_command_t *commands, size_t capacity);
bambu_panel_ui_hit_t bambu_panel_ui_hit_test_home(uint16_t x, uint16_t y);
bambu_panel_ui_state_t bambu_panel_ui_state_default(void);
void bambu_panel_ui_apply_hit(bambu_panel_ui_state_t *state, bambu_panel_ui_hit_t hit);
bambu_panel_ui_action_t bambu_panel_ui_apply_hit_with_action(bambu_panel_ui_state_t *state,
                                                            bambu_panel_ui_hit_t hit);
void bambu_panel_ui_status_text(const bambu_panel_ui_state_t *state,
                                bambu_panel_ui_hit_t hit,
                                char *buffer,
                                size_t buffer_size);
bambu_panel_ui_touch_tracker_t bambu_panel_ui_touch_tracker_default(void);
bambu_panel_ui_touch_event_t bambu_panel_ui_touch_tracker_update(bambu_panel_ui_touch_tracker_t *tracker,
                                                                bool pressed,
                                                                uint16_t x,
                                                                uint16_t y);
int bambu_panel_ui_draw_home(bambu_panel_hw_t *panel);
int bambu_panel_ui_draw_touch_feedback(bambu_panel_hw_t *panel,
                                       uint16_t x,
                                       uint16_t y,
                                       bambu_panel_ui_hit_t hit);
int bambu_panel_ui_draw_state_feedback(bambu_panel_hw_t *panel,
                                       const bambu_panel_ui_state_t *state,
                                       bambu_panel_ui_hit_t hit);

#endif
