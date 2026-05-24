#ifndef BAMBU_TOUCH_DRIVER_H
#define BAMBU_TOUCH_DRIVER_H

#include <stdbool.h>
#include <stdint.h>

#include "board_profile.h"

typedef struct {
    int sda_gpio;
    int scl_gpio;
    int reset_gpio;
    uint32_t i2c_freq_hz;
    uint8_t primary_addr;
    uint8_t fallback_addr;
    uint16_t raw_width;
    uint16_t raw_height;
    uint16_t screen_width;
    uint16_t screen_height;
    bool swap_xy;
    bool invert_x;
    bool invert_y;
} bambu_gt911_config_t;

typedef struct {
    uint16_t x;
    uint16_t y;
} bambu_touch_point_t;

typedef struct {
    bool should_ack;
    bool pressed;
    uint8_t point_count;
    bambu_touch_point_t point;
} bambu_gt911_report_t;

bambu_gt911_config_t bambu_touch_driver_make_gt911_config(const bambu_board_profile_t *profile);
bambu_touch_point_t bambu_touch_driver_transform_gt911_point(const bambu_gt911_config_t *config,
                                                             uint16_t raw_x,
                                                             uint16_t raw_y);
bambu_gt911_report_t bambu_touch_driver_parse_gt911_report(const bambu_gt911_config_t *config,
                                                           uint8_t status,
                                                           const uint8_t point_data[4]);

#endif
