#include <assert.h>
#include <stdint.h>
#include <stddef.h>

#include "board_profile.h"
#include "display_driver.h"
#include "touch_driver.h"

static void test_i80_driver_config_is_derived_from_board_profile(void)
{
    const bambu_board_profile_t *profile = bambu_board_profile_default();
    bambu_i80_display_config_t config = bambu_display_driver_make_i80_config(profile);

    assert(config.width == 800);
    assert(config.height == 480);
    assert(config.pixel_clock_hz == 30000000);
    assert(config.psram_data_alignment == 64);
    assert(config.max_transfer_rows == 80);
    assert(config.trans_queue_depth == 10);
    assert(config.lcd_cmd_bits == 8);
    assert(config.lcd_param_bits == 8);
    assert(config.color_depth_bits == 16);
    assert(config.backlight_gpio == 6);
    assert(config.reset_gpio == 9);
    assert(config.cs_gpio == 12);
    assert(config.dc_gpio == 11);
    assert(config.wr_gpio == 10);
    assert(config.swap_xy);
    assert(config.mirror_x);
    assert(!config.mirror_y);

    const int expected_data_pins[8] = {46, 3, 8, 18, 17, 16, 15, 7};
    for (size_t i = 0; i < 8; ++i) {
        assert(config.data_gpio[i] == expected_data_pins[i]);
    }
}

static void test_hx8369_madctl_matches_display_orientation(void)
{
    const bambu_board_profile_t *profile = bambu_board_profile_default();
    bambu_i80_display_config_t config = bambu_display_driver_make_i80_config(profile);

    assert(bambu_display_driver_make_hx8369_madctl(&config) == 0x60);
}

static void test_gt911_driver_config_is_derived_from_board_profile(void)
{
    const bambu_board_profile_t *profile = bambu_board_profile_default();
    bambu_gt911_config_t config = bambu_touch_driver_make_gt911_config(profile);

    assert(config.sda_gpio == 39);
    assert(config.scl_gpio == 38);
    assert(config.reset_gpio == 4);
    assert(config.i2c_freq_hz == 400000);
    assert(config.primary_addr == 0x5d);
    assert(config.fallback_addr == 0x14);
    assert(config.raw_width == 480);
    assert(config.raw_height == 800);
    assert(config.screen_width == 800);
    assert(config.screen_height == 480);
    assert(config.swap_xy);
    assert(!config.invert_x);
    assert(config.invert_y);
}

static void test_gt911_transform_keeps_points_inside_screen_bounds(void)
{
    const bambu_board_profile_t *profile = bambu_board_profile_default();
    bambu_gt911_config_t config = bambu_touch_driver_make_gt911_config(profile);

    bambu_touch_point_t point = bambu_touch_driver_transform_gt911_point(&config, 10, 20);
    assert(point.x == 779);
    assert(point.y == 10);

    point = bambu_touch_driver_transform_gt911_point(&config, 479, 799);
    assert(point.x == 0);
    assert(point.y == 479);

    point = bambu_touch_driver_transform_gt911_point(&config, 999, 999);
    assert(point.x == 0);
    assert(point.y == 479);
}

static void test_gt911_report_parser_handles_press_release_and_ack(void)
{
    const bambu_board_profile_t *profile = bambu_board_profile_default();
    bambu_gt911_config_t config = bambu_touch_driver_make_gt911_config(profile);

    const uint8_t pressed_point[] = {10, 0, 20, 0};
    bambu_gt911_report_t report = bambu_touch_driver_parse_gt911_report(&config, 0x81, pressed_point);
    assert(report.should_ack);
    assert(report.pressed);
    assert(report.point_count == 1);
    assert(report.point.x == 779);
    assert(report.point.y == 10);

    report = bambu_touch_driver_parse_gt911_report(&config, 0x80, pressed_point);
    assert(report.should_ack);
    assert(!report.pressed);
    assert(report.point_count == 0);

    report = bambu_touch_driver_parse_gt911_report(&config, 0x02, pressed_point);
    assert(!report.should_ack);
    assert(!report.pressed);
    assert(report.point_count == 2);
}

int main(void)
{
    test_i80_driver_config_is_derived_from_board_profile();
    test_hx8369_madctl_matches_display_orientation();
    test_gt911_driver_config_is_derived_from_board_profile();
    test_gt911_transform_keeps_points_inside_screen_bounds();
    test_gt911_report_parser_handles_press_release_and_ack();
    return 0;
}
