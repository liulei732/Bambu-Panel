idf_component_register(
    SRCS "bambu_lvgl_port.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_timer freertos lvgl panel_hw touch_hw
)
