#ifndef BAMBU_TOUCH_HW_H
#define BAMBU_TOUCH_HW_H

#include <stdbool.h>
#include <stdint.h>

#include "driver/i2c_master.h"
#include "esp_err.h"
#include "touch_driver.h"

typedef struct {
    i2c_master_bus_handle_t bus;
    i2c_master_dev_handle_t device;
    bambu_gt911_config_t config;
    uint8_t active_addr;
    char product_id[5];
    uint16_t firmware_version;
    bool initialized;
} bambu_touch_hw_t;

esp_err_t bambu_touch_hw_init(const bambu_gt911_config_t *config, bambu_touch_hw_t *touch);
esp_err_t bambu_touch_hw_init_default(bambu_touch_hw_t *touch);
esp_err_t bambu_touch_hw_read(bambu_touch_hw_t *touch, bambu_gt911_report_t *report);
esp_err_t bambu_touch_hw_deinit(bambu_touch_hw_t *touch);

#endif
