#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "bambu_lvgl_page_model.h"

int main(void)
{
    assert(bambu_lvgl_page_count() == 6);

    assert(strcmp(bambu_lvgl_page_nav_label(BAMBU_LVGL_PAGE_HOME), "Home") == 0);
    assert(strcmp(bambu_lvgl_page_nav_label(BAMBU_LVGL_PAGE_FILES), "Files") == 0);
    assert(strcmp(bambu_lvgl_page_nav_label(BAMBU_LVGL_PAGE_CONTROL), "Control") == 0);
    assert(strcmp(bambu_lvgl_page_nav_label(BAMBU_LVGL_PAGE_AMS), "AMS") == 0);
    assert(strcmp(bambu_lvgl_page_nav_label(BAMBU_LVGL_PAGE_MAINT), "Maint") == 0);
    assert(strcmp(bambu_lvgl_page_nav_label(BAMBU_LVGL_PAGE_SETTINGS), "Set") == 0);

    assert(strcmp(bambu_lvgl_page_title(BAMBU_LVGL_PAGE_HOME), "P1S Control Panel") == 0);
    assert(strcmp(bambu_lvgl_page_title(BAMBU_LVGL_PAGE_CONTROL), "Control") == 0);
    assert(strcmp(bambu_lvgl_page_title(BAMBU_LVGL_PAGE_SETTINGS), "Settings") == 0);

    assert(bambu_lvgl_page_from_nav_index(0) == BAMBU_LVGL_PAGE_HOME);
    assert(bambu_lvgl_page_from_nav_index(3) == BAMBU_LVGL_PAGE_AMS);
    assert(bambu_lvgl_page_from_nav_index(5) == BAMBU_LVGL_PAGE_SETTINGS);
    assert(bambu_lvgl_page_from_nav_index(99) == BAMBU_LVGL_PAGE_HOME);

    return 0;
}
