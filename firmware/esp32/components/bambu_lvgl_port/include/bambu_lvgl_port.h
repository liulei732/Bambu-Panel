#ifndef BAMBU_LVGL_PORT_H
#define BAMBU_LVGL_PORT_H

#include "bambu_panel_hw.h"
#include "bambu_touch_hw.h"
#include "esp_err.h"

esp_err_t bambu_lvgl_port_init(bambu_panel_hw_t *panel, bambu_touch_hw_t *touch);
void bambu_lvgl_port_run(void);

#endif
