#include "bambu_lvgl_page_model.h"

typedef struct {
    const char *nav_label;
    const char *title;
} page_meta_t;

static const page_meta_t k_pages[] = {
    [BAMBU_LVGL_PAGE_HOME] = {"Home", "P1S Control Panel"},
    [BAMBU_LVGL_PAGE_FILES] = {"Files", "Files"},
    [BAMBU_LVGL_PAGE_CONTROL] = {"Control", "Control"},
    [BAMBU_LVGL_PAGE_AMS] = {"AMS", "AMS"},
    [BAMBU_LVGL_PAGE_MAINT] = {"Maint", "Maintenance"},
    [BAMBU_LVGL_PAGE_SETTINGS] = {"Set", "Settings"},
};

size_t bambu_lvgl_page_count(void)
{
    return sizeof(k_pages) / sizeof(k_pages[0]);
}

bambu_lvgl_page_t bambu_lvgl_page_from_nav_index(size_t index)
{
    if (index >= bambu_lvgl_page_count()) {
        return BAMBU_LVGL_PAGE_HOME;
    }
    return (bambu_lvgl_page_t)index;
}

const char *bambu_lvgl_page_nav_label(bambu_lvgl_page_t page)
{
    if ((size_t)page >= bambu_lvgl_page_count()) {
        return k_pages[BAMBU_LVGL_PAGE_HOME].nav_label;
    }
    return k_pages[page].nav_label;
}

const char *bambu_lvgl_page_title(bambu_lvgl_page_t page)
{
    if ((size_t)page >= bambu_lvgl_page_count()) {
        return k_pages[BAMBU_LVGL_PAGE_HOME].title;
    }
    return k_pages[page].title;
}
