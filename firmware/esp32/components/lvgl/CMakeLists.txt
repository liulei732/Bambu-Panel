file(GLOB_RECURSE LVGL_SOURCES
    src/core/*.c
    src/draw/*.c
    src/font/*.c
    src/hal/*.c
    src/layouts/*.c
    src/misc/*.c
    src/themes/*.c
    src/widgets/*.c
)

list(FILTER LVGL_SOURCES EXCLUDE REGEX ".*/src/draw/arm2d/.*")
list(FILTER LVGL_SOURCES EXCLUDE REGEX ".*/src/draw/gd32_ipa/.*")
list(FILTER LVGL_SOURCES EXCLUDE REGEX ".*/src/draw/nxp/.*")
list(FILTER LVGL_SOURCES EXCLUDE REGEX ".*/src/draw/sdl/.*")
list(FILTER LVGL_SOURCES EXCLUDE REGEX ".*/src/draw/stm32_dma2d/.*")
list(FILTER LVGL_SOURCES EXCLUDE REGEX ".*/src/draw/swm341_dma2d/.*")

idf_component_register(
    SRCS ${LVGL_SOURCES}
    INCLUDE_DIRS . src
)

target_compile_definitions(${COMPONENT_LIB} PUBLIC LV_CONF_INCLUDE_SIMPLE)
