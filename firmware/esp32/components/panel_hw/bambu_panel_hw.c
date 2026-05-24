#include "bambu_panel_hw.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "driver/gpio.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "esp_lcd_io_i80.h"
#include "esp_lcd_panel_commands.h"
#include "esp_lcd_panel_io.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "bambu_panel_hw";

#define BAMBU_HX8369_CMD_SET_EXTC 0xB9
#define BAMBU_HX8369_CMD_SET_POWER 0xB1
#define BAMBU_HX8369_CMD_SET_DISPLAY 0xB2
#define BAMBU_HX8369_CMD_SET_DISPLAY_CYC 0xB4
#define BAMBU_HX8369_CMD_SET_VCOM 0xB6
#define BAMBU_HX8369_CMD_SET_GIP 0xD5
#define BAMBU_HX8369_CMD_SET_GAMMA 0xE0
#define BAMBU_HX8369_CMD_SET_DGC 0xC1
#define BAMBU_HX8369_CMD_SET_COLOR 0x2D
#define BAMBU_HX8369_RESET_DELAY_MS 10
#define BAMBU_HX8369_SLEEP_OUT_DELAY_MS 100
#define BAMBU_PANEL_FILL_CHUNK_ROWS 16

static esp_err_t configure_output_gpio(int gpio_num, int initial_level)
{
    if (gpio_num < 0) {
        return ESP_OK;
    }

    gpio_config_t config = {
        .pin_bit_mask = 1ULL << gpio_num,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    ESP_RETURN_ON_ERROR(gpio_config(&config), TAG, "configure gpio %d failed", gpio_num);
    ESP_RETURN_ON_ERROR(gpio_set_level(gpio_num, initial_level), TAG, "set gpio %d failed", gpio_num);
    return ESP_OK;
}

static esp_err_t hx8369_hard_reset(int reset_gpio)
{
    if (reset_gpio < 0) {
        return ESP_OK;
    }

    ESP_RETURN_ON_ERROR(configure_output_gpio(reset_gpio, 1), TAG, "configure LCD reset failed");
    ESP_RETURN_ON_ERROR(gpio_set_level(reset_gpio, 0), TAG, "assert LCD reset failed");
    vTaskDelay(pdMS_TO_TICKS(BAMBU_HX8369_RESET_DELAY_MS));
    ESP_RETURN_ON_ERROR(gpio_set_level(reset_gpio, 1), TAG, "release LCD reset failed");
    vTaskDelay(pdMS_TO_TICKS(BAMBU_HX8369_RESET_DELAY_MS));
    return ESP_OK;
}

static esp_err_t hx8369_tx_param(esp_lcd_panel_io_handle_t io, int command, const void *param, size_t size)
{
    return esp_lcd_panel_io_tx_param(io, command, param, size);
}

static esp_err_t hx8369_send_init_sequence(esp_lcd_panel_io_handle_t io, uint8_t madctl, uint8_t color_depth_bits)
{
    static const uint8_t set_extc[] = {0xFF, 0x83, 0x69};
    static const uint8_t set_power[] = {
        0x01, 0x00, 0x34, 0x06, 0x00, 0x0f, 0x0f, 0x2a, 0x32, 0x3f,
        0x3f, 0x07, 0x23, 0x01, 0xe6, 0xe6, 0xe6, 0xe6, 0xe6,
    };
    static const uint8_t set_display[] = {
        0x00, 0x20, 0x03, 0x03, 0x70, 0x00, 0xff, 0x00, 0x00, 0x00,
        0x00, 0x03, 0x03, 0x00, 0x01,
    };
    static const uint8_t set_display_cyc[] = {0x00, 0x0C, 0xA0, 0x0E, 0x06};
    static const uint8_t set_vcom[] = {0x2C, 0x2C};
    static const uint8_t set_gip[] = {
        0x00, 0x05, 0x03, 0x00, 0x01, 0x09, 0x10, 0x80, 0x37, 0x37, 0x20, 0x31, 0x46,
        0x8a, 0x57, 0x9b, 0x20, 0x31, 0x46, 0x8a, 0x57, 0x9b, 0x07, 0x0f, 0x02, 0x00,
    };
    static const uint8_t set_gamma[] = {
        0x00, 0x08, 0x0d, 0x2d, 0x34, 0x3f, 0x19, 0x38, 0x09, 0x0e, 0x0e, 0x12, 0x14, 0x12, 0x14, 0x13, 0x19,
        0x00, 0x08, 0x0d, 0x2d, 0x34, 0x3f, 0x19, 0x38, 0x09, 0x0e, 0x0e, 0x12, 0x14, 0x12, 0x14, 0x13, 0x19,
    };
    static const uint8_t set_dgc[] = {
        0x01, 0x02, 0x08, 0x12, 0x1a, 0x22, 0x2a, 0x31, 0x36, 0x3f, 0x48, 0x51, 0x58, 0x60, 0x68, 0x70,
        0x78, 0x80, 0x88, 0x90, 0x98, 0xa0, 0xa7, 0xaf, 0xb6, 0xbe, 0xc7, 0xce, 0xd6, 0xde, 0xe6, 0xef,
        0xf5, 0xfb, 0xfc, 0xfe, 0x8c, 0xa4, 0x19, 0xec, 0x1b, 0x4c, 0x40, 0x02, 0x08, 0x12, 0x1a, 0x22,
        0x2a, 0x31, 0x36, 0x3f, 0x48, 0x51, 0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xa0,
        0xa7, 0xaf, 0xb6, 0xbe, 0xc7, 0xce, 0xd6, 0xde, 0xe6, 0xef, 0xf5, 0xfb, 0xfc, 0xfe, 0x8c, 0xa4,
        0x19, 0xec, 0x1b, 0x4c, 0x40, 0x02, 0x08, 0x12, 0x1a, 0x22, 0x2a, 0x31, 0x36, 0x3f, 0x48, 0x51,
        0x58, 0x60, 0x68, 0x70, 0x78, 0x80, 0x88, 0x90, 0x98, 0xa0, 0xa7, 0xaf, 0xb6, 0xbe, 0xc7, 0xce,
        0xd6, 0xde, 0xe6, 0xef, 0xf5, 0xfb, 0xfc, 0xfe, 0x8c, 0xa4, 0x19, 0xec, 0x1b, 0x4c, 0x40,
    };
    static uint8_t color_table[192];
    static bool color_table_ready;
    const uint8_t colmod = color_depth_bits == 18 ? 0x66 : 0x55;

    if (!color_table_ready) {
        for (size_t i = 0; i < 64; ++i) {
            color_table[i] = (uint8_t)(i * 8);
        }
        for (size_t i = 64; i < 128; ++i) {
            color_table[i] = (uint8_t)(i * 4);
        }
        for (size_t i = 128; i < 192; ++i) {
            color_table[i] = (uint8_t)(i * 8);
        }
        color_table_ready = true;
    }

    ESP_RETURN_ON_ERROR(hx8369_tx_param(io, BAMBU_HX8369_CMD_SET_EXTC, set_extc, sizeof(set_extc)), TAG, "send EXTC failed");
    ESP_RETURN_ON_ERROR(hx8369_tx_param(io, BAMBU_HX8369_CMD_SET_POWER, set_power, sizeof(set_power)), TAG, "send power failed");
    ESP_RETURN_ON_ERROR(hx8369_tx_param(io, BAMBU_HX8369_CMD_SET_DISPLAY, set_display, sizeof(set_display)), TAG, "send display failed");
    ESP_RETURN_ON_ERROR(hx8369_tx_param(io, BAMBU_HX8369_CMD_SET_DISPLAY_CYC, set_display_cyc, sizeof(set_display_cyc)), TAG, "send display cyc failed");
    ESP_RETURN_ON_ERROR(hx8369_tx_param(io, BAMBU_HX8369_CMD_SET_VCOM, set_vcom, sizeof(set_vcom)), TAG, "send vcom failed");
    ESP_RETURN_ON_ERROR(hx8369_tx_param(io, BAMBU_HX8369_CMD_SET_GIP, set_gip, sizeof(set_gip)), TAG, "send gip failed");
    ESP_RETURN_ON_ERROR(hx8369_tx_param(io, BAMBU_HX8369_CMD_SET_GAMMA, set_gamma, sizeof(set_gamma)), TAG, "send gamma failed");
    ESP_RETURN_ON_ERROR(hx8369_tx_param(io, BAMBU_HX8369_CMD_SET_DGC, set_dgc, sizeof(set_dgc)), TAG, "send dgc failed");
    ESP_RETURN_ON_ERROR(hx8369_tx_param(io, BAMBU_HX8369_CMD_SET_COLOR, color_table, sizeof(color_table)), TAG, "send color table failed");
    ESP_RETURN_ON_ERROR(hx8369_tx_param(io, LCD_CMD_SLPOUT, NULL, 0), TAG, "exit sleep failed");
    vTaskDelay(pdMS_TO_TICKS(BAMBU_HX8369_SLEEP_OUT_DELAY_MS));
    ESP_RETURN_ON_ERROR(hx8369_tx_param(io, LCD_CMD_MADCTL, &madctl, sizeof(madctl)), TAG, "set madctl failed");
    ESP_RETURN_ON_ERROR(hx8369_tx_param(io, LCD_CMD_COLMOD, &colmod, sizeof(colmod)), TAG, "set colmod failed");
    ESP_RETURN_ON_ERROR(hx8369_tx_param(io, LCD_CMD_DISPON, NULL, 0), TAG, "display on failed");

    return ESP_OK;
}

static esp_err_t hx8369_set_window(esp_lcd_panel_io_handle_t io, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    const uint8_t col[] = {
        (uint8_t)(x0 >> 8), (uint8_t)(x0 & 0xff),
        (uint8_t)(x1 >> 8), (uint8_t)(x1 & 0xff),
    };
    const uint8_t row[] = {
        (uint8_t)(y0 >> 8), (uint8_t)(y0 & 0xff),
        (uint8_t)(y1 >> 8), (uint8_t)(y1 & 0xff),
    };

    ESP_RETURN_ON_ERROR(hx8369_tx_param(io, LCD_CMD_CASET, col, sizeof(col)), TAG, "set columns failed");
    ESP_RETURN_ON_ERROR(hx8369_tx_param(io, LCD_CMD_RASET, row, sizeof(row)), TAG, "set rows failed");
    return ESP_OK;
}

esp_err_t bambu_panel_hw_init(const bambu_i80_display_config_t *config, bambu_panel_hw_t *panel)
{
    ESP_RETURN_ON_FALSE(panel != NULL, ESP_ERR_INVALID_ARG, TAG, "panel is required");

    const bambu_i80_display_config_t default_config = bambu_display_driver_make_i80_config(NULL);
    if (config == NULL) {
        config = &default_config;
    }

    memset(panel, 0, sizeof(*panel));
    panel->display = *config;

    ESP_LOGI(TAG, "Initialize HX8369 I80 LCD %ux%u", config->width, config->height);
    ESP_RETURN_ON_ERROR(configure_output_gpio(config->backlight_gpio, 0), TAG, "configure backlight failed");

    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = config->dc_gpio,
        .wr_gpio_num = config->wr_gpio,
        .clk_src = LCD_CLK_SRC_PLL160M,
        .bus_width = 8,
        .max_transfer_bytes = config->width * config->max_transfer_rows * sizeof(uint16_t),
        .dma_burst_size = config->psram_data_alignment,
    };
    for (size_t i = 0; i < 8; ++i) {
        bus_config.data_gpio_nums[i] = config->data_gpio[i];
    }

    ESP_RETURN_ON_ERROR(esp_lcd_new_i80_bus(&bus_config, &panel->i80_bus), TAG, "create I80 bus failed");

    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = config->cs_gpio,
        .pclk_hz = config->pixel_clock_hz,
        .trans_queue_depth = config->trans_queue_depth,
        .lcd_cmd_bits = config->lcd_cmd_bits,
        .lcd_param_bits = config->lcd_param_bits,
        .dc_levels = {
            .dc_idle_level = 0,
            .dc_cmd_level = 0,
            .dc_dummy_level = 0,
            .dc_data_level = 1,
        },
        .flags = {
            .swap_color_bytes = true,
        },
    };

    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_i80(panel->i80_bus, &io_config, &panel->io), TAG, "create I80 panel IO failed");
    ESP_RETURN_ON_ERROR(hx8369_hard_reset(config->reset_gpio), TAG, "reset HX8369 failed");
    ESP_RETURN_ON_ERROR(hx8369_send_init_sequence(panel->io,
                                                  bambu_display_driver_make_hx8369_madctl(config),
                                                  config->color_depth_bits),
                        TAG, "initialize HX8369 failed");
    ESP_RETURN_ON_ERROR(gpio_set_level(config->backlight_gpio, 1), TAG, "enable backlight failed");

    return ESP_OK;
}

esp_err_t bambu_panel_hw_init_default(bambu_panel_hw_t *panel)
{
    const bambu_i80_display_config_t config = bambu_display_driver_make_i80_config(NULL);
    return bambu_panel_hw_init(&config, panel);
}

esp_err_t bambu_panel_hw_fill_rgb565(bambu_panel_hw_t *panel, uint16_t color)
{
    ESP_RETURN_ON_FALSE(panel != NULL, ESP_ERR_INVALID_ARG, TAG, "panel is required");
    return bambu_panel_hw_fill_rect_rgb565(panel, 0, 0, panel->display.width, panel->display.height, color);
}

esp_err_t bambu_panel_hw_fill_rect_rgb565(bambu_panel_hw_t *panel,
                                          uint16_t x,
                                          uint16_t y,
                                          uint16_t w,
                                          uint16_t h,
                                          uint16_t color)
{
    ESP_RETURN_ON_FALSE(panel != NULL && panel->io != NULL, ESP_ERR_INVALID_ARG, TAG, "panel is not initialized");
    ESP_RETURN_ON_FALSE(w > 0 && h > 0, ESP_ERR_INVALID_ARG, TAG, "empty rectangle");
    ESP_RETURN_ON_FALSE(x < panel->display.width && y < panel->display.height, ESP_ERR_INVALID_ARG, TAG, "rectangle origin out of bounds");
    esp_err_t ret = ESP_OK;

    if ((uint32_t)x + w > panel->display.width) {
        w = (uint16_t)(panel->display.width - x);
    }
    if ((uint32_t)y + h > panel->display.height) {
        h = (uint16_t)(panel->display.height - y);
    }

    uint16_t chunk_rows = h < BAMBU_PANEL_FILL_CHUNK_ROWS ? h : BAMBU_PANEL_FILL_CHUNK_ROWS;
    size_t chunk_pixels = (size_t)w * chunk_rows;
    size_t chunk_bytes = chunk_pixels * sizeof(uint16_t);
    uint16_t *chunk = heap_caps_malloc(chunk_bytes, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    while (chunk == NULL && chunk_rows > 1) {
        chunk_rows /= 2;
        chunk_pixels = (size_t)w * chunk_rows;
        chunk_bytes = chunk_pixels * sizeof(uint16_t);
        chunk = heap_caps_malloc(chunk_bytes, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    }
    ESP_RETURN_ON_FALSE(chunk != NULL, ESP_ERR_NO_MEM, TAG, "allocate rectangle fill chunk failed");

    for (size_t i = 0; i < chunk_pixels; ++i) {
        chunk[i] = color;
    }

    /*
     * This HX8369/I80 panel shows artifacts if one large window is filled by
     * many continued color transfers. Keep each chunk as its own RAMWR window.
     */
    for (uint16_t row_index = 0; row_index < h; row_index = (uint16_t)(row_index + chunk_rows)) {
        const uint16_t remaining_rows = (uint16_t)(h - row_index);
        const uint16_t rows_to_send = remaining_rows < chunk_rows ? remaining_rows : chunk_rows;
        const size_t bytes_to_send = (size_t)w * rows_to_send * sizeof(uint16_t);
        ESP_GOTO_ON_ERROR(hx8369_set_window(panel->io,
                                            x,
                                            (uint16_t)(y + row_index),
                                            (uint16_t)(x + w - 1),
                                            (uint16_t)(y + row_index + rows_to_send - 1)),
                          cleanup, TAG, "set fill chunk window failed");
        ESP_GOTO_ON_ERROR(esp_lcd_panel_io_tx_color(panel->io, LCD_CMD_RAMWR, chunk, bytes_to_send),
                          cleanup, TAG, "fill chunk failed");
    }
    ESP_GOTO_ON_ERROR(hx8369_tx_param(panel->io, LCD_CMD_NOP, NULL, 0), cleanup, TAG, "flush fill failed");

cleanup:
    heap_caps_free(chunk);
    return ret;
}

esp_err_t bambu_panel_hw_deinit(bambu_panel_hw_t *panel)
{
    if (panel == NULL) {
        return ESP_OK;
    }

    esp_err_t ret = ESP_OK;
    if (panel->io != NULL) {
        ret = esp_lcd_panel_io_del(panel->io);
        panel->io = NULL;
    }
    if (panel->i80_bus != NULL) {
        const esp_err_t bus_ret = esp_lcd_del_i80_bus(panel->i80_bus);
        if (ret == ESP_OK) {
            ret = bus_ret;
        }
        panel->i80_bus = NULL;
    }

    return ret;
}
