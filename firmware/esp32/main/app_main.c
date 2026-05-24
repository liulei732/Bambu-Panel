#include <inttypes.h>

#include "bambu_panel_hw.h"
#include "bambu_panel_ui.h"
#include "board_profile.h"
#include "display_driver.h"
#include "display_profile.h"
#include "esp_log.h"
#include "bambu_touch_hw.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "touch_driver.h"

static const char *TAG = "bambu_panel";
static bambu_panel_hw_t s_panel;
static bambu_touch_hw_t s_touch;
static bambu_panel_ui_state_t s_ui_state;
static bambu_panel_ui_touch_tracker_t s_touch_tracker;

static const char *hit_name(bambu_panel_ui_hit_t hit)
{
    switch (hit) {
    case BAMBU_PANEL_UI_HIT_PAUSE:
        return "pause";
    case BAMBU_PANEL_UI_HIT_LIGHT:
        return "light";
    case BAMBU_PANEL_UI_HIT_FAN:
        return "fan";
    case BAMBU_PANEL_UI_HIT_STOP:
        return "stop";
    case BAMBU_PANEL_UI_HIT_NONE:
    default:
        return "none";
    }
}

static const char *display_bus_name(bambu_display_bus_t bus)
{
    switch (bus) {
    case BAMBU_DISPLAY_BUS_I80_8BIT:
        return "i80_8bit";
    case BAMBU_DISPLAY_BUS_RGB:
        return "rgb";
    case BAMBU_DISPLAY_BUS_SPI:
        return "spi";
    case BAMBU_DISPLAY_BUS_MIPI:
        return "mipi";
    default:
        return "unknown";
    }
}

static const char *touch_chip_name(bambu_touch_chip_t chip)
{
    switch (chip) {
    case BAMBU_TOUCH_NONE:
        return "none";
    case BAMBU_TOUCH_GT911:
        return "gt911";
    default:
        return "unknown";
    }
}

static const char *action_name(bambu_panel_ui_action_type_t action)
{
    switch (action) {
    case BAMBU_PANEL_UI_ACTION_PAUSE_PRINT:
        return "pause_print";
    case BAMBU_PANEL_UI_ACTION_RESUME_PRINT:
        return "resume_print";
    case BAMBU_PANEL_UI_ACTION_SET_CHAMBER_LIGHT:
        return "set_chamber_light";
    case BAMBU_PANEL_UI_ACTION_SET_PART_FAN:
        return "set_part_fan";
    case BAMBU_PANEL_UI_ACTION_REQUEST_STOP_CONFIRMATION:
        return "request_stop_confirmation";
    case BAMBU_PANEL_UI_ACTION_NONE:
    default:
        return "none";
    }
}

void app_main(void)
{
    const bambu_board_profile_t *board = bambu_board_profile_default();
    const bambu_display_profile_t *display = bambu_display_profile_default();
    const bambu_i80_display_config_t lcd_config = bambu_display_driver_make_i80_config(board);
    const bambu_gt911_config_t touch_config = bambu_touch_driver_make_gt911_config(board);

    ESP_LOGI(TAG, "Bambu Panel firmware starting");
    ESP_LOGI(TAG, "Board: %s", board->board_id);
    ESP_LOGI(TAG, "Display: %" PRIu16 "x%" PRIu16 " %s pclk=%" PRIu32 "Hz",
             lcd_config.width,
             lcd_config.height,
             display_bus_name(board->display_bus),
             lcd_config.pixel_clock_hz);
    ESP_LOGI(TAG, "LCD pins: D0=%d D1=%d D2=%d D3=%d D4=%d D5=%d D6=%d D7=%d WR=%d CS=%d DC=%d RST=%d BL=%d",
             lcd_config.data_gpio[0],
             lcd_config.data_gpio[1],
             lcd_config.data_gpio[2],
             lcd_config.data_gpio[3],
             lcd_config.data_gpio[4],
             lcd_config.data_gpio[5],
             lcd_config.data_gpio[6],
             lcd_config.data_gpio[7],
             lcd_config.wr_gpio,
             lcd_config.cs_gpio,
             lcd_config.dc_gpio,
             lcd_config.reset_gpio,
             lcd_config.backlight_gpio);
    ESP_LOGI(TAG, "LCD config: color=%" PRIu8 "bit rows=%" PRIu16 " align=%" PRIu16 " swap=%d mirror_x=%d mirror_y=%d",
             lcd_config.color_depth_bits,
             lcd_config.max_transfer_rows,
             lcd_config.psram_data_alignment,
             lcd_config.swap_xy,
             lcd_config.mirror_x,
             lcd_config.mirror_y);
    ESP_LOGI(TAG, "Touch: %s SDA=%d SCL=%d RST=%d addr=0x%02x fallback=0x%02x raw=%" PRIu16 "x%" PRIu16,
             touch_chip_name(board->touch.chip),
             touch_config.sda_gpio,
             touch_config.scl_gpio,
             touch_config.reset_gpio,
             touch_config.primary_addr,
             touch_config.fallback_addr,
             touch_config.raw_width,
             touch_config.raw_height);
    ESP_LOGI(TAG, "UI profile: %s topbar=%" PRIu16 " nav=%" PRIu16 " padding=%" PRIu16 " min_touch=%" PRIu16,
             display->profile_id,
             display->topbar_height,
             display->nav_width,
             display->page_padding,
             display->min_touch_target);

    const esp_err_t panel_ret = bambu_panel_hw_init(&lcd_config, &s_panel);
    s_ui_state = bambu_panel_ui_state_default();
    if (panel_ret == ESP_OK) {
        ESP_ERROR_CHECK(bambu_panel_ui_draw_home(&s_panel));
        ESP_ERROR_CHECK(bambu_panel_ui_draw_state_feedback(&s_panel, &s_ui_state, BAMBU_PANEL_UI_HIT_NONE));
        ESP_LOGI(TAG, "Panel UI home screen drawn");
    } else {
        ESP_LOGE(TAG, "Panel bring-up failed: %s", esp_err_to_name(panel_ret));
    }

    const esp_err_t touch_ret = bambu_touch_hw_init(&touch_config, &s_touch);
    s_touch_tracker = bambu_panel_ui_touch_tracker_default();
    if (touch_ret == ESP_OK) {
        ESP_LOGI(TAG, "Touch bring-up ready");
    } else {
        ESP_LOGE(TAG, "Touch bring-up failed: %s", esp_err_to_name(touch_ret));
    }

    while (true) {
        if (touch_ret == ESP_OK) {
            bambu_gt911_report_t report = {0};
            const esp_err_t read_ret = bambu_touch_hw_read(&s_touch, &report);
            if (read_ret == ESP_OK) {
                const bambu_panel_ui_touch_event_t event =
                    bambu_panel_ui_touch_tracker_update(&s_touch_tracker, report.pressed, report.point.x, report.point.y);
                if (event.type == BAMBU_PANEL_UI_TOUCH_EVENT_PRESS) {
                    ESP_LOGI(TAG, "Touch press x=%" PRIu16 " y=%" PRIu16 " hit=%s",
                             event.x,
                             event.y,
                             hit_name(event.hit));
                    if (panel_ret == ESP_OK) {
                        if (event.hit != BAMBU_PANEL_UI_HIT_NONE) {
                            const bambu_panel_ui_action_t action =
                                bambu_panel_ui_apply_hit_with_action(&s_ui_state, event.hit);
                            ESP_LOGI(TAG, "UI action=%s value=%d", action_name(action.type), action.value);
                            ESP_ERROR_CHECK(bambu_panel_ui_draw_state_feedback(&s_panel, &s_ui_state, event.hit));
                        }
                        ESP_ERROR_CHECK(bambu_panel_ui_draw_touch_feedback(&s_panel, event.x, event.y, event.hit));
                    }
                } else if (event.type == BAMBU_PANEL_UI_TOUCH_EVENT_RELEASE) {
                    ESP_LOGI(TAG, "Touch release hit=%s", hit_name(event.hit));
                    if (panel_ret == ESP_OK) {
                        ESP_ERROR_CHECK(bambu_panel_ui_draw_state_feedback(&s_panel, &s_ui_state, BAMBU_PANEL_UI_HIT_NONE));
                    }
                }
            } else if (read_ret != ESP_OK) {
                ESP_LOGW(TAG, "Touch read failed: %s", esp_err_to_name(read_ret));
            }
        }
        vTaskDelay(pdMS_TO_TICKS(60));
    }
}
