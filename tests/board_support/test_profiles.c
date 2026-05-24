#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "board_profile.h"
#include "display_profile.h"

static void test_default_board_profile_matches_hx8369_gt911_hardware(void)
{
    const bambu_board_profile_t *profile = bambu_board_profile_default();

    assert(profile != NULL);
    assert(strcmp(profile->board_id, "esp32_s3_hx8369_gt911_4p3") == 0);
    assert(profile->chip == BAMBU_CHIP_ESP32_S3);
    assert(profile->display_bus == BAMBU_DISPLAY_BUS_I80_8BIT);
    assert(profile->display.width == 800);
    assert(profile->display.height == 480);
    assert(profile->display.pixel_clock_hz == 30000000);
    assert(profile->display.backlight_gpio == 6);
    assert(profile->display.reset_gpio == 9);
    assert(profile->display.cs_gpio == 12);
    assert(profile->display.dc_gpio == 11);
    assert(profile->display.wr_gpio == 10);

    const int expected_data_pins[8] = {46, 3, 8, 18, 17, 16, 15, 7};
    for (size_t i = 0; i < 8; ++i) {
        assert(profile->display.data_gpio[i] == expected_data_pins[i]);
    }
}

static void test_default_board_profile_matches_gt911_touch_settings(void)
{
    const bambu_board_profile_t *profile = bambu_board_profile_default();

    assert(profile->touch.chip == BAMBU_TOUCH_GT911);
    assert(profile->touch.sda_gpio == 39);
    assert(profile->touch.scl_gpio == 38);
    assert(profile->touch.reset_gpio == 4);
    assert(profile->touch.i2c_freq_hz == 400000);
    assert(profile->touch.primary_addr == 0x5d);
    assert(profile->touch.fallback_addr == 0x14);
    assert(profile->touch.swap_xy);
    assert(!profile->touch.invert_x);
    assert(profile->touch.invert_y);
}

static void test_default_display_profile_matches_800x480_layout_tokens(void)
{
    const bambu_display_profile_t *profile = bambu_display_profile_default();

    assert(profile != NULL);
    assert(strcmp(profile->profile_id, "800x480_landscape") == 0);
    assert(profile->width == 800);
    assert(profile->height == 480);
    assert(profile->topbar_height == 48);
    assert(profile->nav_width == 88);
    assert(profile->page_padding == 14);
    assert(profile->gap == 12);
    assert(profile->min_touch_target == 44);
    assert(profile->font_small == 12);
    assert(profile->font_body == 14);
    assert(profile->font_title == 18);
    assert(profile->font_number == 34);
    assert(profile->supports_dense_ams_view);
}

int main(void)
{
    test_default_board_profile_matches_hx8369_gt911_hardware();
    test_default_board_profile_matches_gt911_touch_settings();
    test_default_display_profile_matches_800x480_layout_tokens();
    return 0;
}
