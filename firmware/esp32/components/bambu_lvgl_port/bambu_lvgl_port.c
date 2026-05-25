#include "bambu_lvgl_port.h"

#include <stdbool.h>
#include <stddef.h>

#include "esp_check.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lvgl.h"

static const char *TAG = "bambu_lvgl_port";

enum {
    LVGL_TICK_PERIOD_MS = 2,
    LVGL_BUFFER_LINES = 32,
};

static bambu_panel_hw_t *s_panel;
static bambu_touch_hw_t *s_touch;
static lv_disp_draw_buf_t s_draw_buf;
static lv_disp_drv_t s_disp_drv;
static lv_indev_drv_t s_indev_drv;
static esp_timer_handle_t s_tick_timer;

static void lvgl_tick_cb(void *arg)
{
    (void)arg;
    lv_tick_inc(LVGL_TICK_PERIOD_MS);
}

static void lvgl_flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    bambu_panel_hw_t *panel = disp_drv == NULL ? NULL : (bambu_panel_hw_t *)disp_drv->user_data;
    const int32_t width = area->x2 - area->x1 + 1;
    const int32_t height = area->y2 - area->y1 + 1;

    if (panel != NULL && width > 0 && height > 0) {
        const esp_err_t ret = bambu_panel_hw_draw_rgb565_bitmap(panel,
                                                                (uint16_t)area->x1,
                                                                (uint16_t)area->y1,
                                                                (uint16_t)width,
                                                                (uint16_t)height,
                                                                (const uint16_t *)color_p);
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "flush failed: %s", esp_err_to_name(ret));
        }
    }

    lv_disp_flush_ready(disp_drv);
}

static void lvgl_touch_read_cb(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    (void)drv;
    bambu_gt911_report_t report = {0};

    if (s_touch == NULL || bambu_touch_hw_read(s_touch, &report) != ESP_OK || !report.pressed) {
        data->state = LV_INDEV_STATE_REL;
        return;
    }

    data->state = LV_INDEV_STATE_PR;
    data->point.x = report.point.x;
    data->point.y = report.point.y;
}

esp_err_t bambu_lvgl_port_init(bambu_panel_hw_t *panel, bambu_touch_hw_t *touch)
{
    ESP_RETURN_ON_FALSE(panel != NULL, ESP_ERR_INVALID_ARG, TAG, "panel is required");

    s_panel = panel;
    s_touch = touch;

    lv_init();

    const size_t pixel_count = (size_t)panel->display.width * LVGL_BUFFER_LINES;
    lv_color_t *buf1 = heap_caps_malloc(pixel_count * sizeof(lv_color_t), MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    lv_color_t *buf2 = heap_caps_malloc(pixel_count * sizeof(lv_color_t), MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    ESP_RETURN_ON_FALSE(buf1 != NULL && buf2 != NULL, ESP_ERR_NO_MEM, TAG, "allocate LVGL buffers failed");

    lv_disp_draw_buf_init(&s_draw_buf, buf1, buf2, (uint32_t)pixel_count);

    lv_disp_drv_init(&s_disp_drv);
    s_disp_drv.hor_res = panel->display.width;
    s_disp_drv.ver_res = panel->display.height;
    s_disp_drv.flush_cb = lvgl_flush_cb;
    s_disp_drv.draw_buf = &s_draw_buf;
    s_disp_drv.user_data = panel;
    lv_disp_drv_register(&s_disp_drv);

    if (touch != NULL) {
        lv_indev_drv_init(&s_indev_drv);
        s_indev_drv.type = LV_INDEV_TYPE_POINTER;
        s_indev_drv.read_cb = lvgl_touch_read_cb;
        lv_indev_drv_register(&s_indev_drv);
    }

    const esp_timer_create_args_t tick_args = {
        .callback = lvgl_tick_cb,
        .name = "lvgl_tick",
    };
    ESP_RETURN_ON_ERROR(esp_timer_create(&tick_args, &s_tick_timer), TAG, "create LVGL tick timer failed");
    ESP_RETURN_ON_ERROR(esp_timer_start_periodic(s_tick_timer, LVGL_TICK_PERIOD_MS * 1000), TAG, "start LVGL tick timer failed");

    return ESP_OK;
}

void bambu_lvgl_port_run(void)
{
    while (true) {
        (void)lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}
