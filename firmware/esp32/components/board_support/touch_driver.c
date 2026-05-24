#include "touch_driver.h"

#include <stddef.h>

#define BAMBU_GT911_STATUS_DATA_READY 0x80
#define BAMBU_GT911_STATUS_POINT_MASK 0x0f

static uint16_t clamp_u16(uint16_t value, uint16_t max_exclusive)
{
    if (max_exclusive == 0) {
        return 0;
    }

    return value >= max_exclusive ? (uint16_t)(max_exclusive - 1) : value;
}

static uint16_t invert_axis(uint16_t value, uint16_t max_exclusive)
{
    value = clamp_u16(value, max_exclusive);
    if (max_exclusive == 0) {
        return 0;
    }

    return (uint16_t)((max_exclusive - 1) - value);
}

bambu_gt911_config_t bambu_touch_driver_make_gt911_config(const bambu_board_profile_t *profile)
{
    if (profile == NULL) {
        profile = bambu_board_profile_default();
    }

    const uint16_t raw_width = profile->touch.swap_xy ? profile->display.height : profile->display.width;
    const uint16_t raw_height = profile->touch.swap_xy ? profile->display.width : profile->display.height;

    return (bambu_gt911_config_t) {
        .sda_gpio = profile->touch.sda_gpio,
        .scl_gpio = profile->touch.scl_gpio,
        .reset_gpio = profile->touch.reset_gpio,
        .i2c_freq_hz = profile->touch.i2c_freq_hz,
        .primary_addr = profile->touch.primary_addr,
        .fallback_addr = profile->touch.fallback_addr,
        .raw_width = raw_width,
        .raw_height = raw_height,
        .screen_width = profile->display.width,
        .screen_height = profile->display.height,
        .swap_xy = profile->touch.swap_xy,
        .invert_x = profile->touch.invert_x,
        .invert_y = profile->touch.invert_y,
    };
}

bambu_touch_point_t bambu_touch_driver_transform_gt911_point(const bambu_gt911_config_t *config,
                                                             uint16_t raw_x,
                                                             uint16_t raw_y)
{
    if (config == NULL) {
        const bambu_gt911_config_t default_config = bambu_touch_driver_make_gt911_config(NULL);
        return bambu_touch_driver_transform_gt911_point(&default_config, raw_x, raw_y);
    }

    uint16_t x = config->invert_x ? invert_axis(raw_x, config->raw_width) : clamp_u16(raw_x, config->raw_width);
    uint16_t y = config->invert_y ? invert_axis(raw_y, config->raw_height) : clamp_u16(raw_y, config->raw_height);

    if (config->swap_xy) {
        const uint16_t swapped = x;
        x = y;
        y = swapped;
    }

    return (bambu_touch_point_t) {
        .x = clamp_u16(x, config->screen_width),
        .y = clamp_u16(y, config->screen_height),
    };
}

bambu_gt911_report_t bambu_touch_driver_parse_gt911_report(const bambu_gt911_config_t *config,
                                                           uint8_t status,
                                                           const uint8_t point_data[4])
{
    bambu_gt911_report_t report = {
        .should_ack = (status & BAMBU_GT911_STATUS_DATA_READY) != 0,
        .pressed = false,
        .point_count = status & BAMBU_GT911_STATUS_POINT_MASK,
        .point = {0, 0},
    };

    if (config == NULL) {
        const bambu_gt911_config_t default_config = bambu_touch_driver_make_gt911_config(NULL);
        return bambu_touch_driver_parse_gt911_report(&default_config, status, point_data);
    }

    if (!report.should_ack || report.point_count != 1 || point_data == NULL) {
        return report;
    }

    const uint16_t raw_x = (uint16_t)(point_data[0] | ((uint16_t)point_data[1] << 8));
    const uint16_t raw_y = (uint16_t)(point_data[2] | ((uint16_t)point_data[3] << 8));
    report.pressed = true;
    report.point = bambu_touch_driver_transform_gt911_point(config, raw_x, raw_y);
    return report;
}
