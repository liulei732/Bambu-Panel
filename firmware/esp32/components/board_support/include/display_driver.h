#ifndef BAMBU_DISPLAY_DRIVER_H
#define BAMBU_DISPLAY_DRIVER_H

#include <stdbool.h>
#include <stdint.h>

#include "board_profile.h"

typedef struct {
    uint16_t width;
    uint16_t height;
    uint32_t pixel_clock_hz;
    uint16_t psram_data_alignment;
    uint16_t max_transfer_rows;
    uint8_t trans_queue_depth;
    uint8_t lcd_cmd_bits;
    uint8_t lcd_param_bits;
    uint8_t color_depth_bits;
    int data_gpio[8];
    int wr_gpio;
    int cs_gpio;
    int dc_gpio;
    int reset_gpio;
    int backlight_gpio;
    bool swap_xy;
    bool mirror_x;
    bool mirror_y;
} bambu_i80_display_config_t;

bambu_i80_display_config_t bambu_display_driver_make_i80_config(const bambu_board_profile_t *profile);
uint8_t bambu_display_driver_make_hx8369_madctl(const bambu_i80_display_config_t *config);

#endif
