#include "bambu_panel_ui.h"

#include <stdbool.h>
#include <string.h>

#include "bambu_panel_hw.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "bambu_panel_ui";

#define UI_COLOR_BG 0x0842
#define UI_COLOR_PANEL_2 0x1906
#define UI_COLOR_TEAL 0x05f5
#define UI_COLOR_RED 0xd929
#define UI_COLOR_WHITE 0xe75d

static const uint8_t FONT_SPACE[7] = {0, 0, 0, 0, 0, 0, 0};

static const uint8_t *font_for_char(char c)
{
    static const uint8_t digits[10][7] = {
        {0x0e, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0e},
        {0x04, 0x0c, 0x04, 0x04, 0x04, 0x04, 0x0e},
        {0x0e, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1f},
        {0x1e, 0x01, 0x01, 0x0e, 0x01, 0x01, 0x1e},
        {0x02, 0x06, 0x0a, 0x12, 0x1f, 0x02, 0x02},
        {0x1f, 0x10, 0x10, 0x1e, 0x01, 0x01, 0x1e},
        {0x06, 0x08, 0x10, 0x1e, 0x11, 0x11, 0x0e},
        {0x1f, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08},
        {0x0e, 0x11, 0x11, 0x0e, 0x11, 0x11, 0x0e},
        {0x0e, 0x11, 0x11, 0x0f, 0x01, 0x02, 0x0c},
    };
    static const uint8_t letters[26][7] = {
        {0x0e, 0x11, 0x11, 0x1f, 0x11, 0x11, 0x11},
        {0x1e, 0x11, 0x11, 0x1e, 0x11, 0x11, 0x1e},
        {0x0e, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0e},
        {0x1e, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1e},
        {0x1f, 0x10, 0x10, 0x1e, 0x10, 0x10, 0x1f},
        {0x1f, 0x10, 0x10, 0x1e, 0x10, 0x10, 0x10},
        {0x0e, 0x11, 0x10, 0x17, 0x11, 0x11, 0x0e},
        {0x11, 0x11, 0x11, 0x1f, 0x11, 0x11, 0x11},
        {0x0e, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0e},
        {0x07, 0x02, 0x02, 0x02, 0x12, 0x12, 0x0c},
        {0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11},
        {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1f},
        {0x11, 0x1b, 0x15, 0x15, 0x11, 0x11, 0x11},
        {0x11, 0x19, 0x15, 0x13, 0x11, 0x11, 0x11},
        {0x0e, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0e},
        {0x1e, 0x11, 0x11, 0x1e, 0x10, 0x10, 0x10},
        {0x0e, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0d},
        {0x1e, 0x11, 0x11, 0x1e, 0x14, 0x12, 0x11},
        {0x0f, 0x10, 0x10, 0x0e, 0x01, 0x01, 0x1e},
        {0x1f, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04},
        {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0e},
        {0x11, 0x11, 0x11, 0x11, 0x11, 0x0a, 0x04},
        {0x11, 0x11, 0x11, 0x15, 0x15, 0x15, 0x0a},
        {0x11, 0x11, 0x0a, 0x04, 0x0a, 0x11, 0x11},
        {0x11, 0x11, 0x0a, 0x04, 0x04, 0x04, 0x04},
        {0x1f, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1f},
    };
    static const uint8_t percent[7] = {0x19, 0x19, 0x02, 0x04, 0x08, 0x13, 0x13};
    static const uint8_t slash[7] = {0x01, 0x01, 0x02, 0x04, 0x08, 0x10, 0x10};
    static const uint8_t dash[7] = {0, 0, 0, 0x1f, 0, 0, 0};
    static const uint8_t plus[7] = {0, 0x04, 0x04, 0x1f, 0x04, 0x04, 0};
    static const uint8_t underscore[7] = {0, 0, 0, 0, 0, 0, 0x1f};

    if (c >= '0' && c <= '9') {
        return digits[c - '0'];
    }
    if (c >= 'a' && c <= 'z') {
        c = (char)(c - 'a' + 'A');
    }
    if (c >= 'A' && c <= 'Z') {
        return letters[c - 'A'];
    }
    if (c == '%') {
        return percent;
    }
    if (c == '/') {
        return slash;
    }
    if (c == '-') {
        return dash;
    }
    if (c == '+') {
        return plus;
    }
    if (c == '_') {
        return underscore;
    }
    return FONT_SPACE;
}

static int draw_char(bambu_panel_hw_t *panel, uint16_t x, uint16_t y, char c, uint8_t scale, uint16_t color)
{
    const uint8_t *glyph = font_for_char(c);
    const uint8_t pixel = scale == 0 ? 1 : scale;

    for (uint8_t row = 0; row < 7; ++row) {
        for (uint8_t col = 0; col < 5; ++col) {
            if ((glyph[row] & (1U << (4 - col))) != 0) {
                const int ret = bambu_panel_hw_fill_rect_rgb565(panel,
                                                                (uint16_t)(x + col * pixel),
                                                                (uint16_t)(y + row * pixel),
                                                                pixel,
                                                                pixel,
                                                                color);
                if (ret != ESP_OK) {
                    return ret;
                }
            }
        }
    }

    return ESP_OK;
}

static int draw_text(bambu_panel_hw_t *panel, const bambu_panel_ui_command_t *command)
{
    const uint8_t scale = command->text_scale == 0 ? 1 : command->text_scale;
    uint16_t x = command->x;

    for (size_t i = 0; i < strlen(command->text); ++i) {
        ESP_RETURN_ON_ERROR(draw_char(panel, x, command->y, command->text[i], scale, command->color), TAG, "draw char failed");
        x = (uint16_t)(x + 6 * scale);
    }

    return ESP_OK;
}

static int draw_scene_commands(bambu_panel_hw_t *panel, bambu_panel_ui_command_t *commands, size_t count)
{
    for (size_t i = 0; i < count; ++i) {
        const bambu_panel_ui_command_t *command = &commands[i];
        if (command->type == BAMBU_PANEL_UI_CMD_RECT) {
            ESP_RETURN_ON_ERROR(bambu_panel_hw_fill_rect_rgb565(panel, command->x, command->y, command->w, command->h, command->color),
                                TAG, "draw rect failed");
        } else if (command->type == BAMBU_PANEL_UI_CMD_TEXT) {
            ESP_RETURN_ON_ERROR(draw_text(panel, command), TAG, "draw text failed");
        }
    }

    return ESP_OK;
}

int bambu_panel_ui_draw_page(bambu_panel_hw_t *panel, const bambu_panel_ui_state_t *state)
{
    ESP_RETURN_ON_FALSE(panel != NULL, ESP_ERR_INVALID_ARG, TAG, "panel is required");

    static bambu_panel_ui_command_t commands[96];
    const size_t count = bambu_panel_ui_page_scene(state, commands, 96);
    return draw_scene_commands(panel, commands, count);
}

int bambu_panel_ui_draw_home(bambu_panel_hw_t *panel)
{
    ESP_RETURN_ON_FALSE(panel != NULL, ESP_ERR_INVALID_ARG, TAG, "panel is required");

    const bambu_panel_ui_state_t state = bambu_panel_ui_state_default();
    return bambu_panel_ui_draw_page(panel, &state);
}

static int draw_outline(bambu_panel_hw_t *panel, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    ESP_RETURN_ON_ERROR(bambu_panel_hw_fill_rect_rgb565(panel, x, y, w, 3, color), TAG, "draw outline top failed");
    ESP_RETURN_ON_ERROR(bambu_panel_hw_fill_rect_rgb565(panel, x, (uint16_t)(y + h - 3), w, 3, color), TAG, "draw outline bottom failed");
    ESP_RETURN_ON_ERROR(bambu_panel_hw_fill_rect_rgb565(panel, x, y, 3, h, color), TAG, "draw outline left failed");
    ESP_RETURN_ON_ERROR(bambu_panel_hw_fill_rect_rgb565(panel, (uint16_t)(x + w - 3), y, 3, h, color), TAG, "draw outline right failed");
    return ESP_OK;
}

static int draw_button(bambu_panel_hw_t *panel,
                       uint16_t x,
                       uint16_t y,
                       uint16_t w,
                       uint16_t h,
                       const char *text,
                       uint16_t fill_color,
                       uint16_t text_color)
{
    ESP_RETURN_ON_ERROR(bambu_panel_hw_fill_rect_rgb565(panel, x, y, w, h, fill_color), TAG, "draw button failed");

    const bambu_panel_ui_command_t command = {
        .type = BAMBU_PANEL_UI_CMD_TEXT,
        .x = (uint16_t)(x + 20),
        .y = (uint16_t)(y + 15),
        .color = text_color,
        .text_scale = 1,
    };
    bambu_panel_ui_command_t text_command = command;
    (void)strncpy(text_command.text, text, sizeof(text_command.text) - 1);
    text_command.text[sizeof(text_command.text) - 1] = '\0';
    return draw_text(panel, &text_command);
}

static int draw_control_buttons(bambu_panel_hw_t *panel, const bambu_panel_ui_state_t *state)
{
    const uint16_t pause_fill = state->paused ? UI_COLOR_PANEL_2 : UI_COLOR_TEAL;
    const uint16_t pause_text = state->paused ? UI_COLOR_WHITE : UI_COLOR_BG;
    ESP_RETURN_ON_ERROR(draw_button(panel, 584, 282, 76, 44, state->paused ? "RESUME" : "PAUSE", pause_fill, pause_text),
                        TAG,
                        "draw pause button failed");
    ESP_RETURN_ON_ERROR(draw_button(panel, 678, 282, 76, 44, state->chamber_light_on ? "ON" : "LIGHT", UI_COLOR_PANEL_2, UI_COLOR_WHITE),
                        TAG,
                        "draw light button failed");
    ESP_RETURN_ON_ERROR(draw_button(panel, 584, 348, 76, 44, state->part_fan_percent == 0 ? "FAN0" : "FAN", UI_COLOR_PANEL_2, UI_COLOR_WHITE),
                        TAG,
                        "draw fan button failed");
    ESP_RETURN_ON_ERROR(draw_button(panel, 678, 348, 76, 44, state->stop_confirm_pending ? "SURE" : "STOP", UI_COLOR_RED, UI_COLOR_WHITE),
                        TAG,
                        "draw stop button failed");

    if (state->paused) {
        ESP_RETURN_ON_ERROR(draw_outline(panel, 584, 282, 76, 44, UI_COLOR_TEAL), TAG, "draw paused state failed");
    }
    if (state->chamber_light_on) {
        ESP_RETURN_ON_ERROR(draw_outline(panel, 678, 282, 76, 44, UI_COLOR_TEAL), TAG, "draw light state failed");
    }
    if (state->part_fan_percent == 0 || state->part_fan_percent == 100) {
        ESP_RETURN_ON_ERROR(draw_outline(panel, 584, 348, 76, 44, UI_COLOR_TEAL), TAG, "draw fan state failed");
    }
    if (state->stop_confirm_pending) {
        ESP_RETURN_ON_ERROR(draw_outline(panel, 678, 348, 76, 44, UI_COLOR_RED), TAG, "draw stop state failed");
    }

    return ESP_OK;
}

int bambu_panel_ui_draw_touch_feedback(bambu_panel_hw_t *panel,
                                       uint16_t x,
                                       uint16_t y,
                                       bambu_panel_ui_hit_t hit)
{
    ESP_RETURN_ON_FALSE(panel != NULL, ESP_ERR_INVALID_ARG, TAG, "panel is required");

    const uint16_t marker_x = x > 5 ? (uint16_t)(x - 5) : 0;
    const uint16_t marker_y = y > 5 ? (uint16_t)(y - 5) : 0;
    const uint16_t marker_color = hit == BAMBU_PANEL_UI_HIT_STOP ? UI_COLOR_RED : UI_COLOR_TEAL;
    ESP_RETURN_ON_ERROR(bambu_panel_hw_fill_rect_rgb565(panel, 742, 436, 46, 32, UI_COLOR_BG), TAG, "clear marker failed");
    if (hit == BAMBU_PANEL_UI_HIT_NONE) {
        ESP_RETURN_ON_ERROR(bambu_panel_hw_fill_rect_rgb565(panel, marker_x, marker_y, 10, 10, marker_color), TAG, "draw marker failed");
    }

    if (hit == BAMBU_PANEL_UI_HIT_NONE) {
        return ESP_OK;
    }

    if (hit == BAMBU_PANEL_UI_HIT_PAUSE) {
        return draw_outline(panel, 584, 282, 76, 44, UI_COLOR_WHITE);
    }
    if (hit == BAMBU_PANEL_UI_HIT_LIGHT) {
        return draw_outline(panel, 678, 282, 76, 44, UI_COLOR_WHITE);
    }
    if (hit == BAMBU_PANEL_UI_HIT_FAN) {
        return draw_outline(panel, 584, 348, 76, 44, UI_COLOR_WHITE);
    }
    if (hit == BAMBU_PANEL_UI_HIT_STOP) {
        return draw_outline(panel, 678, 348, 76, 44, UI_COLOR_RED);
    }

    return ESP_OK;
}

int bambu_panel_ui_draw_state_feedback(bambu_panel_hw_t *panel,
                                       const bambu_panel_ui_state_t *state,
                                       bambu_panel_ui_hit_t hit)
{
    ESP_RETURN_ON_FALSE(panel != NULL, ESP_ERR_INVALID_ARG, TAG, "panel is required");
    ESP_RETURN_ON_FALSE(state != NULL, ESP_ERR_INVALID_ARG, TAG, "state is required");

    char status[32];
    bambu_panel_ui_status_text(state, hit, status, sizeof(status));

    const uint16_t status_color = state->stop_confirm_pending ? UI_COLOR_RED : UI_COLOR_TEAL;
    ESP_RETURN_ON_ERROR(bambu_panel_hw_fill_rect_rgb565(panel, 108, 432, 590, 40, UI_COLOR_BG), TAG, "clear state failed");

    const bambu_panel_ui_command_t command = {
        .type = BAMBU_PANEL_UI_CMD_TEXT,
        .x = 110,
        .y = 444,
        .color = status_color,
        .text_scale = 1,
    };
    bambu_panel_ui_command_t text_command = command;
    (void)strncpy(text_command.text, status, sizeof(text_command.text) - 1);
    text_command.text[sizeof(text_command.text) - 1] = '\0';
    ESP_RETURN_ON_ERROR(draw_text(panel, &text_command), TAG, "draw state text failed");

    return draw_control_buttons(panel, state);
}
