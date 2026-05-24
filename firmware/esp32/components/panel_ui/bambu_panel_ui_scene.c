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

size_t bambu_panel_ui_home_scene(bambu_panel_ui_command_t *commands, size_t capacity)
{
    if (commands == NULL || capacity == 0) {
        return 0;
    }

    size_t count = 0;
    push_rect(commands, capacity, &count, 0, 0, 800, 480, C_BG);
    push_rect(commands, capacity, &count, 0, 0, 88, 480, C_NAV);
    push_rect(commands, capacity, &count, 86, 0, 2, 480, C_LINE);
    push_text(commands, capacity, &count, 18, 20, "P1S", 3, C_TEXT);
    push_rect(commands, capacity, &count, 18, 74, 52, 4, C_TEAL);

    const char *nav[] = {"HOME", "FILES", "CTRL", "AMS", "SET"};
    for (size_t i = 0; i < 5; ++i) {
        const uint16_t y = (uint16_t)(112 + i * 62);
        push_rect(commands, capacity, &count, 12, y, 64, 34, i == 0 ? C_TEAL : C_PANEL_2);
        push_text(commands, capacity, &count, 23, (uint16_t)(y + 11), nav[i], 1, i == 0 ? C_BG : C_MUTED);
    }

    push_text(commands, capacity, &count, 110, 20, "PRINTING", 2, C_TEAL);
    push_text(commands, capacity, &count, 300, 22, "BENCHY_TOP_PLATE", 1, C_TEXT);
    push_text(commands, capacity, &count, 668, 22, "P1S ONLINE", 1, C_GREEN);

    push_rect(commands, capacity, &count, 108, 58, 438, 178, C_PANEL);
    push_text(commands, capacity, &count, 130, 82, "42%", 5, C_TEXT);
    push_text(commands, capacity, &count, 130, 142, "01H 24M LEFT", 2, C_MUTED);
    push_rect(commands, capacity, &count, 130, 188, 360, 16, C_PANEL_2);
    push_rect(commands, capacity, &count, 130, 188, 151, 16, C_TEAL);

    push_rect(commands, capacity, &count, 562, 58, 214, 178, C_PANEL);
    push_text(commands, capacity, &count, 584, 82, "NOZ 218/220", 2, C_TEXT);
    push_text(commands, capacity, &count, 584, 122, "BED  60/60", 2, C_TEXT);
    push_text(commands, capacity, &count, 584, 162, "FAN  70%", 2, C_TEXT);
    push_text(commands, capacity, &count, 584, 202, "SPD  100%", 2, C_TEXT);

    push_rect(commands, capacity, &count, 108, 252, 438, 166, C_PANEL);
    push_text(commands, capacity, &count, 130, 276, "AMS 1", 2, C_TEXT);
    push_text(commands, capacity, &count, 338, 276, "AMS 2", 2, C_TEXT);
    const uint16_t slot_colors[] = {C_TEAL, C_AMBER, C_RED, C_GREEN, C_TEAL, C_AMBER, C_GREEN, C_RED};
    for (size_t i = 0; i < 8; ++i) {
        const uint16_t group_x = i < 4 ? 130 : 338;
        const uint16_t x = (uint16_t)(group_x + (i % 4) * 50);
        push_rect(commands, capacity, &count, x, 318, 46, 34, slot_colors[i]);
        push_text(commands, capacity, &count, (uint16_t)(x + 17), 329, i == 0 ? "1" : i == 1 ? "2" : i == 2 ? "3" : i == 3 ? "4" : i == 4 ? "5" : i == 5 ? "6" : i == 6 ? "7" : "8", 1, C_BG);
    }
    push_text(commands, capacity, &count, 130, 378, "PLA-CF  BLACK", 1, C_MUTED);
    push_text(commands, capacity, &count, 338, 378, "PLA    WHITE", 1, C_MUTED);

    push_rect(commands, capacity, &count, 562, 252, 214, 166, C_PANEL);
    push_rect(commands, capacity, &count, 584, 282, 76, 44, C_TEAL);
    push_text(commands, capacity, &count, 604, 297, "PAUSE", 1, C_BG);
    push_rect(commands, capacity, &count, 678, 282, 76, 44, C_PANEL_2);
    push_text(commands, capacity, &count, 700, 297, "LIGHT", 1, C_TEXT);
    push_rect(commands, capacity, &count, 584, 348, 76, 44, C_PANEL_2);
    push_text(commands, capacity, &count, 606, 363, "FAN", 1, C_TEXT);
    push_rect(commands, capacity, &count, 678, 348, 76, 44, C_RED);
    push_text(commands, capacity, &count, 702, 363, "STOP", 1, C_TEXT);

    push_text(commands, capacity, &count, 110, 444, "LAN CONTROL READY", 1, C_MUTED);
    push_rect(commands, capacity, &count, 716, 444, 60, 8, C_TEAL);

    return count;
}

static bool point_in_rect(uint16_t px, uint16_t py, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    return px >= x && px < (uint16_t)(x + w) && py >= y && py < (uint16_t)(y + h);
}

bambu_panel_ui_hit_t bambu_panel_ui_hit_test_home(uint16_t x, uint16_t y)
{
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
    };
}

void bambu_panel_ui_apply_hit(bambu_panel_ui_state_t *state, bambu_panel_ui_hit_t hit)
{
    if (state == NULL) {
        return;
    }

    if (hit != BAMBU_PANEL_UI_HIT_NONE && hit != BAMBU_PANEL_UI_HIT_STOP) {
        state->stop_confirm_pending = false;
    }

    switch (hit) {
    case BAMBU_PANEL_UI_HIT_PAUSE:
        state->paused = !state->paused;
        break;
    case BAMBU_PANEL_UI_HIT_LIGHT:
        state->chamber_light_on = !state->chamber_light_on;
        break;
    case BAMBU_PANEL_UI_HIT_FAN:
        if (state->part_fan_percent < 70) {
            state->part_fan_percent = 70;
        } else if (state->part_fan_percent < 100) {
            state->part_fan_percent = 100;
        } else {
            state->part_fan_percent = 0;
        }
        break;
    case BAMBU_PANEL_UI_HIT_STOP:
        state->stop_confirm_pending = true;
        break;
    case BAMBU_PANEL_UI_HIT_NONE:
    default:
        break;
    }
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
