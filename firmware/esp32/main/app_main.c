#include <inttypes.h>

#include "bambu_lvgl_port.h"
#include "bambu_lvgl_ui.h"
#include "bambu_panel_hw.h"
#include "bambu_touch_hw.h"
#include "board_profile.h"
#include "display_driver.h"
#include "display_profile.h"
#include "esp_log.h"
#include "touch_driver.h"

static const char *TAG = "bambu_panel";
static bambu_panel_hw_t s_panel;
static bambu_touch_hw_t s_touch;

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

    ESP_ERROR_CHECK(bambu_panel_hw_init(&lcd_config, &s_panel));

    const esp_err_t touch_ret = bambu_touch_hw_init(&touch_config, &s_touch);
    if (touch_ret == ESP_OK) {
        ESP_LOGI(TAG, "Touch bring-up ready");
    } else {
        ESP_LOGE(TAG, "Touch bring-up failed: %s", esp_err_to_name(touch_ret));
    }

    ESP_ERROR_CHECK(bambu_lvgl_port_init(&s_panel, touch_ret == ESP_OK ? &s_touch : NULL));
    bambu_lvgl_ui_show_home();
    ESP_LOGI(TAG, "LVGL home screen ready");

    bambu_lvgl_port_run();
}
