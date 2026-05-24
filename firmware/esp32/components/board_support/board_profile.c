#include "board_profile.h"

static const bambu_board_profile_t k_default_board_profile = {
    .board_id = "esp32_s3_hx8369_gt911_4p3",
    .chip = BAMBU_CHIP_ESP32_S3,
    .has_psram = false,
    .flash_size_mb = 8,
    .display_bus = BAMBU_DISPLAY_BUS_I80_8BIT,
    .display = {
        .width = 800,
        .height = 480,
        .pixel_clock_hz = 30 * 1000 * 1000,
        .psram_data_alignment = 64,
        .data_gpio = {46, 3, 8, 18, 17, 16, 15, 7},
        .wr_gpio = 10,
        .cs_gpio = 12,
        .dc_gpio = 11,
        .reset_gpio = 9,
        .backlight_gpio = 6,
        .swap_xy = true,
        .mirror_x = true,
        .mirror_y = false,
    },
    .touch = {
        .chip = BAMBU_TOUCH_GT911,
        .sda_gpio = 39,
        .scl_gpio = 38,
        .reset_gpio = 4,
        .i2c_freq_hz = 400000,
        .primary_addr = 0x5d,
        .fallback_addr = 0x14,
        .swap_xy = true,
        .invert_x = false,
        .invert_y = true,
    },
    .default_display_profile = "800x480_landscape",
};

const bambu_board_profile_t *bambu_board_profile_default(void)
{
    return &k_default_board_profile;
}
