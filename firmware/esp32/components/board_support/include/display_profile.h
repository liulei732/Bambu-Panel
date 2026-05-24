#ifndef BAMBU_DISPLAY_PROFILE_H
#define BAMBU_DISPLAY_PROFILE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    const char *profile_id;
    uint16_t width;
    uint16_t height;
    uint16_t topbar_height;
    uint16_t nav_width;
    uint16_t page_padding;
    uint16_t gap;
    uint16_t min_touch_target;
    uint16_t font_small;
    uint16_t font_body;
    uint16_t font_title;
    uint16_t font_number;
    bool supports_dense_ams_view;
} bambu_display_profile_t;

const bambu_display_profile_t *bambu_display_profile_default(void);

#endif
