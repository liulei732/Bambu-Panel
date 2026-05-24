#ifndef BAMBU_BOARD_PROFILE_H
#define BAMBU_BOARD_PROFILE_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    BAMBU_CHIP_ESP32 = 0,
    BAMBU_CHIP_ESP32_S3,
    BAMBU_CHIP_ESP32_P4,
} bambu_chip_t;

typedef enum {
    BAMBU_DISPLAY_BUS_I80_8BIT = 0,
    BAMBU_DISPLAY_BUS_RGB,
    BAMBU_DISPLAY_BUS_SPI,
    BAMBU_DISPLAY_BUS_MIPI,
} bambu_display_bus_t;

typedef enum {
    BAMBU_TOUCH_NONE = 0,
    BAMBU_TOUCH_GT911,
} bambu_touch_chip_t;

typedef struct {
    uint16_t width;
    uint16_t height;
    uint32_t pixel_clock_hz;
    uint16_t psram_data_alignment;
    int data_gpio[8];
    int wr_gpio;
    int cs_gpio;
    int dc_gpio;
    int reset_gpio;
    int backlight_gpio;
    bool swap_xy;
    bool mirror_x;
    bool mirror_y;
} bambu_lcd_i80_profile_t;

typedef struct {
    bambu_touch_chip_t chip;
    int sda_gpio;
    int scl_gpio;
    int reset_gpio;
    uint32_t i2c_freq_hz;
    uint8_t primary_addr;
    uint8_t fallback_addr;
    bool swap_xy;
    bool invert_x;
    bool invert_y;
} bambu_touch_profile_t;

typedef struct {
    const char *board_id;
    bambu_chip_t chip;
    bool has_psram;
    uint32_t flash_size_mb;
    bambu_display_bus_t display_bus;
    bambu_lcd_i80_profile_t display;
    bambu_touch_profile_t touch;
    const char *default_display_profile;
} bambu_board_profile_t;

const bambu_board_profile_t *bambu_board_profile_default(void);

#endif
