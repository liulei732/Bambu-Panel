#include "bambu_panel_ui.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define RGB565(r, g, b) (uint16_t)((((r) & 0xf8) << 8) | (((g) & 0xfc) << 3) | ((b) >> 3))

enum {
    C_BG = RGB565(8, 8, 18),
    C_NAV = RGB565(12, 16, 26),
    C_PANEL = RGB565(20, 25, 38),
    C_PANEL_2 = RGB565(28, 34, 48),
    C_LINE = RGB565(52, 66, 82),
    C_TEXT = RGB565(225, 234, 236),
    C_MUTED = RGB565(124, 139, 152),
    C_TEAL = RGB565(0, 188, 172),
    C_AMBER = RGB565(238, 180, 64),
    C_RED = RGB565(220, 74, 78),
    C_GREEN = RGB565(80, 210, 140),
};

static bool push_rect(bambu_panel_ui_command_t *commands,
                      size_t capacity,
                      size_t *count,
                      uint16_t x,
                      uint16_t y,
                      uint16_t w,
                      uint16_t h,
                      uint16_t color)
{
    if (*count >= capacity) {
        return false;
    }

    commands[*count] = (bambu_panel_ui_command_t) {
        .type = BAMBU_PANEL_UI_CMD_RECT,
        .x = x,
        .y = y,
        .w = w,
        .h = h,
        .color = color,
    };
    ++(*count);
    return true;
}

static bool push_text(bambu_panel_ui_command_t *commands,
                      size_t capacity,
                      size_t *count,
                      uint16_t x,
                      uint16_t y,
                      const char *text,
                      uint8_t scale,
                      uint16_t color)
{
    if (*count >= capacity) {
        return false;
    }

    commands[*count] = (bambu_panel_ui_command_t) {
        .type = BAMBU_PANEL_UI_CMD_TEXT,
        .x = x,
        .y = y,
        .w = 0,
        .h = 0,
        .color = color,
        .text_scale = scale,
    };
    (void)strncpy(commands[*count].text, text, sizeof(commands[*count].text) - 1);
    commands[*count].text[sizeof(commands[*count].text) - 1] = '\0';
    ++(*count);
    return true;
}

static void push_nav(bambu_panel_ui_command_t *commands,
                     size_t capacity,
                     size_t *count,
                     bambu_panel_ui_page_t active_page)
{
    push_rect(commands, capacity, count, 0, 0, 800, 480, C_BG);
    push_rect(commands, capacity, count, 0, 0, 88, 480, C_NAV);
    push_rect(commands, capacity, count, 86, 0, 2, 480, C_LINE);
    push_text(commands, capacity, count, 18, 20, "P1S", 3, C_TEXT);
    push_rect(commands, capacity, count, 18, 74, 52, 4, C_TEAL);

    const char *nav[] = {"HOME", "FILES", "CTRL", "AMS", "SET"};
    for (size_t i = 0; i < 5; ++i) {
        const uint16_t y = (uint16_t)(112 + i * 62);
        const bool active = active_page == (bambu_panel_ui_page_t)i;
        push_rect(commands, capacity, count, 12, y, 64, 34, active ? C_TEAL : C_PANEL_2);
        push_text(commands, capacity, count, 23, (uint16_t)(y + 11), nav[i], 1, active ? C_BG : C_MUTED);
    }
}

static void push_home_page(bambu_panel_ui_command_t *commands, size_t capacity, size_t *count)
{
    push_text(commands, capacity, count, 110, 20, "PRINTING", 2, C_TEAL);
    push_text(commands, capacity, count, 300, 22, "BENCHY_TOP_PLATE", 1, C_TEXT);
    push_text(commands, capacity, count, 668, 22, "P1S ONLINE", 1, C_GREEN);

    push_rect(commands, capacity, count, 108, 58, 438, 178, C_PANEL);
    push_text(commands, capacity, count, 130, 82, "42%", 5, C_TEXT);
    push_text(commands, capacity, count, 130, 142, "01H 24M LEFT", 2, C_MUTED);
    push_rect(commands, capacity, count, 130, 188, 360, 16, C_PANEL_2);
    push_rect(commands, capacity, count, 130, 188, 151, 16, C_TEAL);

    push_rect(commands, capacity, count, 562, 58, 214, 178, C_PANEL);
    push_text(commands, capacity, count, 584, 82, "NOZ 218/220", 2, C_TEXT);
    push_text(commands, capacity, count, 584, 122, "BED  60/60", 2, C_TEXT);
    push_text(commands, capacity, count, 584, 162, "FAN  70%", 2, C_TEXT);
    push_text(commands, capacity, count, 584, 202, "SPD  100%", 2, C_TEXT);

    push_rect(commands, capacity, count, 108, 252, 438, 166, C_PANEL);
    push_text(commands, capacity, count, 130, 276, "AMS 1", 2, C_TEXT);
    push_text(commands, capacity, count, 338, 276, "AMS 2", 2, C_TEXT);
    const uint16_t slot_colors[] = {C_TEAL, C_AMBER, C_RED, C_GREEN, C_TEAL, C_AMBER, C_GREEN, C_RED};
    for (size_t i = 0; i < 8; ++i) {
        const uint16_t group_x = i < 4 ? 130 : 338;
        const uint16_t x = (uint16_t)(group_x + (i % 4) * 50);
        push_rect(commands, capacity, count, x, 318, 46, 34, slot_colors[i]);
        push_text(commands, capacity, count, (uint16_t)(x + 17), 329, i == 0 ? "1" : i == 1 ? "2" : i == 2 ? "3" : i == 3 ? "4" : i == 4 ? "5" : i == 5 ? "6" : i == 6 ? "7" : "8", 1, C_BG);
    }
    push_text(commands, capacity, count, 130, 378, "PLA-CF  BLACK", 1, C_MUTED);
    push_text(commands, capacity, count, 338, 378, "PLA    WHITE", 1, C_MUTED);

    push_rect(commands, capacity, count, 562, 252, 214, 166, C_PANEL);
    push_rect(commands, capacity, count, 584, 282, 76, 44, C_TEAL);
    push_text(commands, capacity, count, 604, 297, "PAUSE", 1, C_BG);
    push_rect(commands, capacity, count, 678, 282, 76, 44, C_PANEL_2);
    push_text(commands, capacity, count, 700, 297, "LIGHT", 1, C_TEXT);
    push_rect(commands, capacity, count, 584, 348, 76, 44, C_PANEL_2);
    push_text(commands, capacity, count, 606, 363, "FAN", 1, C_TEXT);
    push_rect(commands, capacity, count, 678, 348, 76, 44, C_RED);
    push_text(commands, capacity, count, 702, 363, "STOP", 1, C_TEXT);

    push_text(commands, capacity, count, 110, 444, "LAN CONTROL READY", 1, C_MUTED);
    push_rect(commands, capacity, count, 716, 444, 60, 8, C_TEAL);
}

static void push_slider_card(bambu_panel_ui_command_t *commands,
                             size_t capacity,
                             size_t *count,
                             uint16_t x,
                             uint16_t y,
                             uint16_t h,
                             const char *label,
                             const char *meta,
                             const char *value,
                             const char *unit,
                             const char *minus,
                             const char *plus,
                             uint16_t fill_w,
                             uint16_t accent)
{
    const uint16_t step_y = (uint16_t)(y + h - 44);
    const uint16_t slider_y = (uint16_t)(step_y + 14);

    push_rect(commands, capacity, count, x, y, 210, h, C_PANEL);
    push_text(commands, capacity, count, (uint16_t)(x + 18), (uint16_t)(y + 18), label, 2, C_TEXT);
    push_text(commands, capacity, count, (uint16_t)(x + 132), (uint16_t)(y + 22), meta, 1, C_MUTED);
    push_text(commands, capacity, count, (uint16_t)(x + 20), (uint16_t)(y + 58), value, 4, accent);
    push_text(commands, capacity, count, (uint16_t)(x + 104), (uint16_t)(y + 70), unit, 2, C_MUTED);
    push_rect(commands, capacity, count, (uint16_t)(x + 18), step_y, 42, 30, C_PANEL_2);
    push_text(commands, capacity, count, (uint16_t)(x + 29), (uint16_t)(step_y + 10), minus, 1, C_TEXT);
    push_rect(commands, capacity, count, (uint16_t)(x + 74), slider_y, 62, 8, C_PANEL_2);
    push_rect(commands, capacity, count, (uint16_t)(x + 74), slider_y, fill_w, 8, accent);
    push_rect(commands, capacity, count, (uint16_t)(x + 150), step_y, 42, 30, C_PANEL_2);
    push_text(commands, capacity, count, (uint16_t)(x + 161), (uint16_t)(step_y + 10), plus, 1, C_TEXT);
}

static void push_ctrl_page(bambu_panel_ui_command_t *commands, size_t capacity, size_t *count)
{
    push_text(commands, capacity, count, 110, 20, "CONTROL", 2, C_TEAL);
    push_text(commands, capacity, count, 300, 22, "MANUAL PANEL", 1, C_TEXT);

    push_slider_card(commands, capacity, count, 108, 58, 150, "NOZZLE", "NOW219", "220", "C", "-5", "+5", 46, C_TEAL);
    push_slider_card(commands, capacity, count, 337, 58, 150, "BED", "NOW55", "55", "C", "-5", "+5", 29, C_AMBER);
    push_slider_card(commands, capacity, count, 566, 58, 150, "FAN", "PART", "70", "%", "-10", "+10", 43, C_GREEN);
    push_slider_card(commands, capacity, count, 108, 230, 130, "SPEED", "STD", "100", "%", "-5", "+5", 31, C_TEAL);
    push_slider_card(commands, capacity, count, 337, 230, 130, "FLOW", "CAREFUL", "100", "%", "-1", "+1", 31, C_AMBER);

    push_rect(commands, capacity, count, 566, 230, 210, 130, C_PANEL);
    push_text(commands, capacity, count, 584, 248, "PREHEAT", 2, C_TEXT);
    push_text(commands, capacity, count, 698, 252, "IDLE", 1, C_MUTED);
    push_rect(commands, capacity, count, 584, 288, 44, 30, C_TEAL);
    push_text(commands, capacity, count, 595, 298, "PLA", 1, C_BG);
    push_rect(commands, capacity, count, 638, 288, 50, 30, C_PANEL_2);
    push_text(commands, capacity, count, 650, 298, "PETG", 1, C_TEXT);
    push_rect(commands, capacity, count, 698, 288, 44, 30, C_PANEL_2);
    push_text(commands, capacity, count, 709, 298, "ABS", 1, C_TEXT);
    push_rect(commands, capacity, count, 584, 330, 158, 30, C_PANEL_2);
    push_text(commands, capacity, count, 614, 340, "CUSTOM", 1, C_TEXT);

    push_text(commands, capacity, count, 110, 444, "LOCAL UI ONLY", 1, C_MUTED);
}

static void push_files_page(bambu_panel_ui_command_t *commands, size_t capacity, size_t *count)
{
    push_text(commands, capacity, count, 110, 20, "FILES", 2, C_TEAL);
    push_rect(commands, capacity, count, 108, 58, 668, 360, C_PANEL);
    push_text(commands, capacity, count, 130, 90, "RECENT JOBS", 2, C_TEXT);
    push_text(commands, capacity, count, 130, 150, "BENCHY_TOP_PLATE", 1, C_MUTED);
    push_text(commands, capacity, count, 130, 196, "CALIBRATION_CUBE", 1, C_MUTED);
    push_text(commands, capacity, count, 130, 242, "AMS_COLOR_TEST", 1, C_MUTED);
    push_text(commands, capacity, count, 110, 444, "FILE BROWSER PLACEHOLDER", 1, C_MUTED);
}

static void push_ams_page(bambu_panel_ui_command_t *commands, size_t capacity, size_t *count)
{
    push_text(commands, capacity, count, 110, 20, "AMS STATUS", 2, C_TEAL);
    push_text(commands, capacity, count, 300, 22, "2 AMS 8 SLOTS", 1, C_TEXT);
    push_rect(commands, capacity, count, 108, 58, 668, 360, C_PANEL);
    const uint16_t slot_colors[] = {C_TEAL, C_AMBER, C_RED, C_GREEN, C_TEAL, C_AMBER, C_GREEN, C_RED};
    for (size_t i = 0; i < 8; ++i) {
        const uint16_t x = (uint16_t)(130 + (i % 4) * 150);
        const uint16_t y = (uint16_t)(110 + (i / 4) * 150);
        push_rect(commands, capacity, count, x, y, 92, 70, slot_colors[i]);
        push_text(commands, capacity, count, (uint16_t)(x + 36), (uint16_t)(y + 24), i == 0 ? "1" : i == 1 ? "2" : i == 2 ? "3" : i == 3 ? "4" : i == 4 ? "5" : i == 5 ? "6" : i == 6 ? "7" : "8", 2, C_BG);
        push_text(commands, capacity, count, x, (uint16_t)(y + 88), i < 4 ? "AMS 1" : "AMS 2", 1, C_MUTED);
    }
    push_text(commands, capacity, count, 110, 444, "SLOT DATA PLACEHOLDER", 1, C_MUTED);
}

static void push_set_page(bambu_panel_ui_command_t *commands, size_t capacity, size_t *count)
{
    push_text(commands, capacity, count, 110, 20, "SETTINGS", 2, C_TEAL);
    push_rect(commands, capacity, count, 108, 58, 668, 360, C_PANEL);
    push_text(commands, capacity, count, 130, 90, "NETWORK", 2, C_TEXT);
    push_text(commands, capacity, count, 130, 136, "WIFI NOT SET", 1, C_MUTED);
    push_text(commands, capacity, count, 130, 200, "PRINTER", 2, C_TEXT);
    push_text(commands, capacity, count, 130, 246, "P1S LAN MODE", 1, C_MUTED);
    push_text(commands, capacity, count, 130, 310, "DISPLAY", 2, C_TEXT);
    push_text(commands, capacity, count, 130, 356, "BRIGHTNESS 100", 1, C_MUTED);
    push_text(commands, capacity, count, 110, 444, "BAMBU PANEL FW", 1, C_MUTED);
}

size_t bambu_panel_ui_page_scene(const bambu_panel_ui_state_t *state,
                                 bambu_panel_ui_command_t *commands,
                                 size_t capacity)
{
    if (commands == NULL || capacity == 0) {
        return 0;
    }

    size_t count = 0;
    const bambu_panel_ui_page_t page = state == NULL ? BAMBU_PANEL_UI_PAGE_HOME : state->current_page;
    push_nav(commands, capacity, &count, page);

    switch (page) {
    case BAMBU_PANEL_UI_PAGE_CTRL:
        push_ctrl_page(commands, capacity, &count);
        break;
    case BAMBU_PANEL_UI_PAGE_FILES:
        push_files_page(commands, capacity, &count);
        break;
    case BAMBU_PANEL_UI_PAGE_AMS:
        push_ams_page(commands, capacity, &count);
        break;
    case BAMBU_PANEL_UI_PAGE_SET:
        push_set_page(commands, capacity, &count);
        break;
    case BAMBU_PANEL_UI_PAGE_HOME:
    default:
        push_home_page(commands, capacity, &count);
        break;
    }

    return count;
}

size_t bambu_panel_ui_home_scene(bambu_panel_ui_command_t *commands, size_t capacity)
{
    const bambu_panel_ui_state_t state = bambu_panel_ui_state_default();
    return bambu_panel_ui_page_scene(&state, commands, capacity);
}

static bool point_in_rect(uint16_t px, uint16_t py, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    return px >= x && px < (uint16_t)(x + w) && py >= y && py < (uint16_t)(y + h);
}

bambu_panel_ui_hit_t bambu_panel_ui_hit_test_home(uint16_t x, uint16_t y)
{
    if (point_in_rect(x, y, 12, 112, 64, 34)) {
        return BAMBU_PANEL_UI_HIT_NAV_HOME;
    }
    if (point_in_rect(x, y, 12, 174, 64, 34)) {
        return BAMBU_PANEL_UI_HIT_NAV_FILES;
    }
    if (point_in_rect(x, y, 12, 236, 64, 34)) {
        return BAMBU_PANEL_UI_HIT_NAV_CTRL;
    }
    if (point_in_rect(x, y, 12, 298, 64, 34)) {
        return BAMBU_PANEL_UI_HIT_NAV_AMS;
    }
    if (point_in_rect(x, y, 12, 360, 64, 34)) {
        return BAMBU_PANEL_UI_HIT_NAV_SET;
    }
    if (point_in_rect(x, y, 584, 282, 76, 44)) {
        return BAMBU_PANEL_UI_HIT_PAUSE;
    }
    if (point_in_rect(x, y, 678, 282, 76, 44)) {
        return BAMBU_PANEL_UI_HIT_LIGHT;
    }
    if (point_in_rect(x, y, 584, 348, 76, 44)) {
        return BAMBU_PANEL_UI_HIT_FAN;
    }
    if (point_in_rect(x, y, 678, 348, 76, 44)) {
        return BAMBU_PANEL_UI_HIT_STOP;
    }

    return BAMBU_PANEL_UI_HIT_NONE;
}

bambu_panel_ui_state_t bambu_panel_ui_state_default(void)
{
    return (bambu_panel_ui_state_t) {
        .paused = false,
        .chamber_light_on = false,
        .part_fan_percent = 70,
        .stop_confirm_pending = false,
        .current_page = BAMBU_PANEL_UI_PAGE_HOME,
    };
}

void bambu_panel_ui_apply_hit(bambu_panel_ui_state_t *state, bambu_panel_ui_hit_t hit)
{
    (void)bambu_panel_ui_apply_hit_with_action(state, hit);
}

bambu_panel_ui_action_t bambu_panel_ui_apply_hit_with_action(bambu_panel_ui_state_t *state,
                                                            bambu_panel_ui_hit_t hit)
{
    if (state == NULL) {
        return (bambu_panel_ui_action_t) {
            .type = BAMBU_PANEL_UI_ACTION_NONE,
            .value = 0,
        };
    }

    bambu_panel_ui_action_t action = {
        .type = BAMBU_PANEL_UI_ACTION_NONE,
        .value = 0,
    };

    if (hit != BAMBU_PANEL_UI_HIT_NONE && hit != BAMBU_PANEL_UI_HIT_STOP) {
        state->stop_confirm_pending = false;
    }

    switch (hit) {
    case BAMBU_PANEL_UI_HIT_NAV_HOME:
        state->current_page = BAMBU_PANEL_UI_PAGE_HOME;
        state->stop_confirm_pending = false;
        action.type = BAMBU_PANEL_UI_ACTION_SWITCH_PAGE;
        action.value = BAMBU_PANEL_UI_PAGE_HOME;
        break;
    case BAMBU_PANEL_UI_HIT_NAV_FILES:
        state->current_page = BAMBU_PANEL_UI_PAGE_FILES;
        state->stop_confirm_pending = false;
        action.type = BAMBU_PANEL_UI_ACTION_SWITCH_PAGE;
        action.value = BAMBU_PANEL_UI_PAGE_FILES;
        break;
    case BAMBU_PANEL_UI_HIT_NAV_CTRL:
        state->current_page = BAMBU_PANEL_UI_PAGE_CTRL;
        state->stop_confirm_pending = false;
        action.type = BAMBU_PANEL_UI_ACTION_SWITCH_PAGE;
        action.value = BAMBU_PANEL_UI_PAGE_CTRL;
        break;
    case BAMBU_PANEL_UI_HIT_NAV_AMS:
        state->current_page = BAMBU_PANEL_UI_PAGE_AMS;
        state->stop_confirm_pending = false;
        action.type = BAMBU_PANEL_UI_ACTION_SWITCH_PAGE;
        action.value = BAMBU_PANEL_UI_PAGE_AMS;
        break;
    case BAMBU_PANEL_UI_HIT_NAV_SET:
        state->current_page = BAMBU_PANEL_UI_PAGE_SET;
        state->stop_confirm_pending = false;
        action.type = BAMBU_PANEL_UI_ACTION_SWITCH_PAGE;
        action.value = BAMBU_PANEL_UI_PAGE_SET;
        break;
    case BAMBU_PANEL_UI_HIT_PAUSE:
        if (state->current_page != BAMBU_PANEL_UI_PAGE_HOME) {
            break;
        }
        state->paused = !state->paused;
        action.type = state->paused ? BAMBU_PANEL_UI_ACTION_PAUSE_PRINT : BAMBU_PANEL_UI_ACTION_RESUME_PRINT;
        break;
    case BAMBU_PANEL_UI_HIT_LIGHT:
        if (state->current_page != BAMBU_PANEL_UI_PAGE_HOME) {
            break;
        }
        state->chamber_light_on = !state->chamber_light_on;
        action.type = BAMBU_PANEL_UI_ACTION_SET_CHAMBER_LIGHT;
        action.value = state->chamber_light_on ? 1 : 0;
        break;
    case BAMBU_PANEL_UI_HIT_FAN:
        if (state->current_page != BAMBU_PANEL_UI_PAGE_HOME) {
            break;
        }
        if (state->part_fan_percent < 70) {
            state->part_fan_percent = 70;
        } else if (state->part_fan_percent < 100) {
            state->part_fan_percent = 100;
        } else {
            state->part_fan_percent = 0;
        }
        action.type = BAMBU_PANEL_UI_ACTION_SET_PART_FAN;
        action.value = state->part_fan_percent;
        break;
    case BAMBU_PANEL_UI_HIT_STOP:
        if (state->current_page != BAMBU_PANEL_UI_PAGE_HOME) {
            break;
        }
        state->stop_confirm_pending = true;
        action.type = BAMBU_PANEL_UI_ACTION_REQUEST_STOP_CONFIRMATION;
        break;
    case BAMBU_PANEL_UI_HIT_NONE:
    default:
        break;
    }

    return action;
}

void bambu_panel_ui_status_text(const bambu_panel_ui_state_t *state,
                                bambu_panel_ui_hit_t hit,
                                char *buffer,
                                size_t buffer_size)
{
    if (buffer == NULL || buffer_size == 0) {
        return;
    }

    if (state == NULL) {
        buffer[0] = '\0';
        return;
    }

    if (hit == BAMBU_PANEL_UI_HIT_STOP || state->stop_confirm_pending) {
        (void)snprintf(buffer, buffer_size, "STOP CONFIRM");
        return;
    }

    (void)snprintf(buffer,
                   buffer_size,
                   "%s LAMP %s FAN %" PRIu8,
                   state->paused ? "PAUSED" : "RUN",
                   state->chamber_light_on ? "ON" : "OFF",
                   state->part_fan_percent);
}

bambu_panel_ui_touch_tracker_t bambu_panel_ui_touch_tracker_default(void)
{
    return (bambu_panel_ui_touch_tracker_t) {
        .pressed = false,
        .release_samples = 0,
        .press_x = 0,
        .press_y = 0,
        .press_hit = BAMBU_PANEL_UI_HIT_NONE,
    };
}

bambu_panel_ui_touch_event_t bambu_panel_ui_touch_tracker_update(bambu_panel_ui_touch_tracker_t *tracker,
                                                                bool pressed,
                                                                uint16_t x,
                                                                uint16_t y)
{
    bambu_panel_ui_touch_event_t event = {
        .type = BAMBU_PANEL_UI_TOUCH_EVENT_NONE,
        .x = x,
        .y = y,
        .hit = BAMBU_PANEL_UI_HIT_NONE,
    };

    if (tracker == NULL) {
        return event;
    }

    if (pressed && !tracker->pressed) {
        tracker->pressed = true;
        tracker->release_samples = 0;
        tracker->press_x = x;
        tracker->press_y = y;
        tracker->press_hit = bambu_panel_ui_hit_test_home(x, y);

        event.type = BAMBU_PANEL_UI_TOUCH_EVENT_PRESS;
        event.hit = tracker->press_hit;
        return event;
    }

    if (pressed && tracker->pressed) {
        tracker->release_samples = 0;
        return event;
    }

    if (!pressed && tracker->pressed) {
        ++tracker->release_samples;
        if (tracker->release_samples < 2) {
            return event;
        }

        tracker->pressed = false;
        tracker->release_samples = 0;

        event.type = BAMBU_PANEL_UI_TOUCH_EVENT_RELEASE;
        event.x = tracker->press_x;
        event.y = tracker->press_y;
        event.hit = tracker->press_hit;
        tracker->press_hit = BAMBU_PANEL_UI_HIT_NONE;
        return event;
    }

    return event;
}
