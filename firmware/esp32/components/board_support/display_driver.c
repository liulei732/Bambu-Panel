#include "display_driver.h"

#include <stddef.h>

#define BAMBU_I80_DEFAULT_COLOR_DEPTH_BITS 16
#define BAMBU_I80_DEFAULT_MAX_TRANSFER_ROWS 80
#define BAMBU_I80_DEFAULT_TRANS_QUEUE_DEPTH 10
#define BAMBU_I80_DEFAULT_LCD_CMD_BITS 8
#define BAMBU_I80_DEFAULT_LCD_PARAM_BITS 8
#define BAMBU_HX8369_MADCTL_MV_BIT (1U << 5)
#define BAMBU_HX8369_MADCTL_MX_BIT (1U << 6)
#define BAMBU_HX8369_MADCTL_MY_BIT (1U << 7)

bambu_i80_display_config_t bambu_display_driver_make_i80_config(const bambu_board_profile_t *profile)
{
    if (profile == NULL) {
        profile = bambu_board_profile_default();
    }

    bambu_i80_display_config_t config = {
        .width = profile->display.width,
        .height = profile->display.height,
        .pixel_clock_hz = profile->display.pixel_clock_hz,
        .psram_data_alignment = profile->display.psram_data_alignment,
        .max_transfer_rows = BAMBU_I80_DEFAULT_MAX_TRANSFER_ROWS,
        .trans_queue_depth = BAMBU_I80_DEFAULT_TRANS_QUEUE_DEPTH,
        .lcd_cmd_bits = BAMBU_I80_DEFAULT_LCD_CMD_BITS,
        .lcd_param_bits = BAMBU_I80_DEFAULT_LCD_PARAM_BITS,
        .color_depth_bits = BAMBU_I80_DEFAULT_COLOR_DEPTH_BITS,
        .wr_gpio = profile->display.wr_gpio,
        .cs_gpio = profile->display.cs_gpio,
        .dc_gpio = profile->display.dc_gpio,
        .reset_gpio = profile->display.reset_gpio,
        .backlight_gpio = profile->display.backlight_gpio,
        .swap_xy = profile->display.swap_xy,
        .mirror_x = profile->display.mirror_x,
        .mirror_y = profile->display.mirror_y,
    };

    for (size_t i = 0; i < 8; ++i) {
        config.data_gpio[i] = profile->display.data_gpio[i];
    }

    return config;
}

uint8_t bambu_display_driver_make_hx8369_madctl(const bambu_i80_display_config_t *config)
{
    if (config == NULL) {
        const bambu_i80_display_config_t default_config = bambu_display_driver_make_i80_config(NULL);
        return bambu_display_driver_make_hx8369_madctl(&default_config);
    }

    uint8_t madctl = 0;
    if (config->swap_xy) {
        madctl |= BAMBU_HX8369_MADCTL_MV_BIT;
    }
    if (config->mirror_x) {
        madctl |= BAMBU_HX8369_MADCTL_MX_BIT;
    }
    if (config->mirror_y) {
        madctl |= BAMBU_HX8369_MADCTL_MY_BIT;
    }

    return madctl;
}
