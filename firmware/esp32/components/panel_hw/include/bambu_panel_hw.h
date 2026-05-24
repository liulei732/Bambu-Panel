#ifndef BAMBU_PANEL_HW_H
#define BAMBU_PANEL_HW_H

#include <stdint.h>

#include "display_driver.h"
#include "esp_err.h"
#include "esp_lcd_io_i80.h"
#include "esp_lcd_types.h"

typedef struct bambu_panel_hw_t {
    esp_lcd_i80_bus_handle_t i80_bus;
    esp_lcd_panel_io_handle_t io;
    bambu_i80_display_config_t display;
} bambu_panel_hw_t;

esp_err_t bambu_panel_hw_init(const bambu_i80_display_config_t *config, bambu_panel_hw_t *panel);
esp_err_t bambu_panel_hw_init_default(bambu_panel_hw_t *panel);
esp_err_t bambu_panel_hw_fill_rgb565(bambu_panel_hw_t *panel, uint16_t color);
esp_err_t bambu_panel_hw_fill_rect_rgb565(bambu_panel_hw_t *panel,
                                          uint16_t x,
                                          uint16_t y,
                                          uint16_t w,
                                          uint16_t h,
                                          uint16_t color);
esp_err_t bambu_panel_hw_deinit(bambu_panel_hw_t *panel);

#endif
