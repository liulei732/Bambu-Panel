#include "display_profile.h"

static const bambu_display_profile_t k_default_display_profile = {
    .profile_id = "800x480_landscape",
    .width = 800,
    .height = 480,
    .topbar_height = 48,
    .nav_width = 88,
    .page_padding = 14,
    .gap = 12,
    .min_touch_target = 44,
    .font_small = 12,
    .font_body = 14,
    .font_title = 18,
    .font_number = 34,
    .supports_dense_ams_view = true,
};

const bambu_display_profile_t *bambu_display_profile_default(void)
{
    return &k_default_display_profile;
}
