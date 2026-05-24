#include "bambu_touch_hw.h"

#include <stddef.h>
#include <string.h>

#include "driver/gpio.h"
#include "esp_check.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "bambu_touch_hw";

#define GT911_PRODUCT_ID 0x8140
#define GT911_FIRMWARE_VER 0x8144
#define GT911_STATUS_REG 0x814E
#define GT911_PT1_X_COORD_L 0x8150
#define GT911_RESET_HIGH_DELAY_MS 20
#define GT911_RESET_LOW_DELAY_MS 20
#define GT911_RESET_READY_DELAY_MS 100
#define GT911_I2C_TIMEOUT_MS 50

static esp_err_t gt911_reset(int reset_gpio)
{
    if (reset_gpio < 0) {
        return ESP_OK;
    }

    gpio_config_t config = {
        .pin_bit_mask = 1ULL << reset_gpio,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    ESP_RETURN_ON_ERROR(gpio_config(&config), TAG, "configure GT911 reset failed");
    ESP_RETURN_ON_ERROR(gpio_set_level(reset_gpio, 1), TAG, "set GT911 reset high failed");
    vTaskDelay(pdMS_TO_TICKS(GT911_RESET_HIGH_DELAY_MS));
    ESP_RETURN_ON_ERROR(gpio_set_level(reset_gpio, 0), TAG, "set GT911 reset low failed");
    vTaskDelay(pdMS_TO_TICKS(GT911_RESET_LOW_DELAY_MS));
    ESP_RETURN_ON_ERROR(gpio_set_level(reset_gpio, 1), TAG, "release GT911 reset failed");
    vTaskDelay(pdMS_TO_TICKS(GT911_RESET_READY_DELAY_MS));
    return ESP_OK;
}

static void gt911_reg_addr(uint16_t reg, uint8_t out[2])
{
    out[0] = (uint8_t)(reg >> 8);
    out[1] = (uint8_t)(reg & 0xff);
}

static esp_err_t gt911_read_reg(i2c_master_dev_handle_t device, uint16_t reg, uint8_t *data, size_t len)
{
    uint8_t addr[2];
    gt911_reg_addr(reg, addr);
    return i2c_master_transmit_receive(device, addr, sizeof(addr), data, len, GT911_I2C_TIMEOUT_MS);
}

static esp_err_t gt911_write_u8(i2c_master_dev_handle_t device, uint16_t reg, uint8_t value)
{
    uint8_t packet[3];
    gt911_reg_addr(reg, packet);
    packet[2] = value;
    return i2c_master_transmit(device, packet, sizeof(packet), GT911_I2C_TIMEOUT_MS);
}

static esp_err_t gt911_add_device(i2c_master_bus_handle_t bus, const bambu_gt911_config_t *config, uint8_t addr, i2c_master_dev_handle_t *device)
{
    i2c_device_config_t device_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = addr,
        .scl_speed_hz = config->i2c_freq_hz,
    };

    return i2c_master_bus_add_device(bus, &device_config, device);
}

static esp_err_t gt911_probe_and_add_device(i2c_master_bus_handle_t bus,
                                            const bambu_gt911_config_t *config,
                                            uint8_t *active_addr,
                                            i2c_master_dev_handle_t *device)
{
    const uint8_t addrs[] = {config->primary_addr, config->fallback_addr};
    esp_err_t last_error = ESP_ERR_NOT_FOUND;

    for (size_t i = 0; i < sizeof(addrs); ++i) {
        const uint8_t addr = addrs[i];
        last_error = i2c_master_probe(bus, addr, GT911_I2C_TIMEOUT_MS);
        if (last_error == ESP_OK) {
            ESP_RETURN_ON_ERROR(gt911_add_device(bus, config, addr, device), TAG, "add GT911 device failed");
            *active_addr = addr;
            return ESP_OK;
        }
        ESP_LOGW(TAG, "No GT911 ACK at 0x%02x: %s", addr, esp_err_to_name(last_error));
    }

    return last_error;
}

esp_err_t bambu_touch_hw_init(const bambu_gt911_config_t *config, bambu_touch_hw_t *touch)
{
    ESP_RETURN_ON_FALSE(touch != NULL, ESP_ERR_INVALID_ARG, TAG, "touch handle is required");

    const bambu_gt911_config_t default_config = bambu_touch_driver_make_gt911_config(NULL);
    if (config == NULL) {
        config = &default_config;
    }

    memset(touch, 0, sizeof(*touch));
    touch->config = *config;

    ESP_RETURN_ON_ERROR(gt911_reset(config->reset_gpio), TAG, "reset GT911 failed");

    i2c_master_bus_config_t bus_config = {
        .i2c_port = -1,
        .sda_io_num = config->sda_gpio,
        .scl_io_num = config->scl_gpio,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags = {
            .enable_internal_pullup = true,
        },
    };
    ESP_RETURN_ON_ERROR(i2c_new_master_bus(&bus_config, &touch->bus), TAG, "create GT911 I2C bus failed");
    ESP_RETURN_ON_ERROR(gt911_probe_and_add_device(touch->bus, config, &touch->active_addr, &touch->device),
                        TAG, "probe GT911 failed");

    uint8_t product[4] = {0};
    ESP_RETURN_ON_ERROR(gt911_read_reg(touch->device, GT911_PRODUCT_ID, product, sizeof(product)), TAG, "read GT911 product id failed");
    memcpy(touch->product_id, product, sizeof(product));
    touch->product_id[4] = '\0';

    uint8_t firmware[2] = {0};
    ESP_RETURN_ON_ERROR(gt911_read_reg(touch->device, GT911_FIRMWARE_VER, firmware, sizeof(firmware)), TAG, "read GT911 firmware failed");
    touch->firmware_version = (uint16_t)(firmware[0] | ((uint16_t)firmware[1] << 8));
    touch->initialized = true;

    ESP_LOGI(TAG, "GT911 ready addr=0x%02x product=%s firmware=0x%04x",
             touch->active_addr,
             touch->product_id,
             touch->firmware_version);

    return ESP_OK;
}

esp_err_t bambu_touch_hw_init_default(bambu_touch_hw_t *touch)
{
    const bambu_gt911_config_t config = bambu_touch_driver_make_gt911_config(NULL);
    return bambu_touch_hw_init(&config, touch);
}

esp_err_t bambu_touch_hw_read(bambu_touch_hw_t *touch, bambu_gt911_report_t *report)
{
    ESP_RETURN_ON_FALSE(touch != NULL && touch->initialized && touch->device != NULL, ESP_ERR_INVALID_ARG, TAG, "touch is not initialized");
    ESP_RETURN_ON_FALSE(report != NULL, ESP_ERR_INVALID_ARG, TAG, "report is required");

    uint8_t status = 0;
    ESP_RETURN_ON_ERROR(gt911_read_reg(touch->device, GT911_STATUS_REG, &status, sizeof(status)), TAG, "read GT911 status failed");

    uint8_t point_data[4] = {0};
    if ((status & 0x80) != 0 && (status & 0x0f) == 1) {
        ESP_RETURN_ON_ERROR(gt911_read_reg(touch->device, GT911_PT1_X_COORD_L, point_data, sizeof(point_data)), TAG, "read GT911 point failed");
    }

    *report = bambu_touch_driver_parse_gt911_report(&touch->config, status, point_data);
    if (report->should_ack) {
        ESP_RETURN_ON_ERROR(gt911_write_u8(touch->device, GT911_STATUS_REG, 0x00), TAG, "ack GT911 status failed");
    }

    return ESP_OK;
}

esp_err_t bambu_touch_hw_deinit(bambu_touch_hw_t *touch)
{
    if (touch == NULL) {
        return ESP_OK;
    }

    esp_err_t ret = ESP_OK;
    if (touch->device != NULL) {
        ret = i2c_master_bus_rm_device(touch->device);
        touch->device = NULL;
    }
    if (touch->bus != NULL) {
        const esp_err_t bus_ret = i2c_del_master_bus(touch->bus);
        if (ret == ESP_OK) {
            ret = bus_ret;
        }
        touch->bus = NULL;
    }
    touch->initialized = false;
    return ret;
}
