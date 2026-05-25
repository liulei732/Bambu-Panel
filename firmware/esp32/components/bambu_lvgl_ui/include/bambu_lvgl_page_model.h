#ifndef BAMBU_LVGL_PAGE_MODEL_H
#define BAMBU_LVGL_PAGE_MODEL_H

#include <stddef.h>

typedef enum {
    BAMBU_LVGL_PAGE_HOME = 0,
    BAMBU_LVGL_PAGE_FILES,
    BAMBU_LVGL_PAGE_CONTROL,
    BAMBU_LVGL_PAGE_AMS,
    BAMBU_LVGL_PAGE_MAINT,
    BAMBU_LVGL_PAGE_SETTINGS,
} bambu_lvgl_page_t;

size_t bambu_lvgl_page_count(void);
bambu_lvgl_page_t bambu_lvgl_page_from_nav_index(size_t index);
const char *bambu_lvgl_page_nav_label(bambu_lvgl_page_t page);
const char *bambu_lvgl_page_title(bambu_lvgl_page_t page);

#endif
