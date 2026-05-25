#include "bambu_lvgl_ui.h"

#include <stddef.h>

#include "lvgl.h"

enum {
    C_BG = 0x080812,
    C_TOP = 0x0d1320,
    C_RAIL = 0x0c101a,
    C_PANEL = 0x141c2b,
    C_PANEL_2 = 0x1d2a42,
    C_PANEL_3 = 0x24334d,
    C_TEXT = 0xe8eef2,
    C_MUTED = 0x93a4b7,
    C_DIM = 0x617184,
    C_TEAL = 0x0891b2,
    C_TEAL_2 = 0x22d3ee,
    C_AMBER = 0xf59e0b,
    C_RED = 0xef4444,
    C_GREEN = 0x22c55e,
};

static lv_style_t s_screen;
static lv_style_t s_topbar;
static lv_style_t s_rail;
static lv_style_t s_card;
static lv_style_t s_card_inset;
static lv_style_t s_nav;
static lv_style_t s_nav_active;
static lv_style_t s_btn;
static lv_style_t s_btn_primary;
static lv_style_t s_btn_danger;
static lv_style_t s_title;
static lv_style_t s_body;
static lv_style_t s_small;
static lv_style_t s_value;
static bool s_styles_ready;

static void style_text(lv_style_t *style, uint32_t color, const lv_font_t *font)
{
    lv_style_init(style);
    lv_style_set_text_color(style, lv_color_hex(color));
    lv_style_set_text_font(style, font);
}

static void init_styles(void)
{
    if (s_styles_ready) {
        return;
    }

    lv_style_init(&s_screen);
    lv_style_set_bg_color(&s_screen, lv_color_hex(C_BG));
    lv_style_set_bg_opa(&s_screen, LV_OPA_COVER);
    lv_style_set_border_width(&s_screen, 0);
    lv_style_set_pad_all(&s_screen, 0);

    lv_style_init(&s_topbar);
    lv_style_set_bg_color(&s_topbar, lv_color_hex(C_TOP));
    lv_style_set_bg_opa(&s_topbar, LV_OPA_COVER);
    lv_style_set_border_width(&s_topbar, 0);
    lv_style_set_pad_all(&s_topbar, 0);

    lv_style_init(&s_rail);
    lv_style_set_bg_color(&s_rail, lv_color_hex(C_RAIL));
    lv_style_set_bg_opa(&s_rail, LV_OPA_COVER);
    lv_style_set_border_color(&s_rail, lv_color_hex(0x253045));
    lv_style_set_border_side(&s_rail, LV_BORDER_SIDE_RIGHT);
    lv_style_set_border_width(&s_rail, 1);
    lv_style_set_pad_all(&s_rail, 0);

    lv_style_init(&s_card);
    lv_style_set_bg_color(&s_card, lv_color_hex(C_PANEL));
    lv_style_set_bg_opa(&s_card, LV_OPA_COVER);
    lv_style_set_radius(&s_card, 8);
    lv_style_set_border_width(&s_card, 0);
    lv_style_set_pad_all(&s_card, 14);

    lv_style_init(&s_card_inset);
    lv_style_set_bg_color(&s_card_inset, lv_color_hex(C_PANEL_2));
    lv_style_set_bg_opa(&s_card_inset, LV_OPA_COVER);
    lv_style_set_radius(&s_card_inset, 8);
    lv_style_set_border_width(&s_card_inset, 0);
    lv_style_set_pad_all(&s_card_inset, 10);

    lv_style_init(&s_nav);
    lv_style_set_bg_color(&s_nav, lv_color_hex(C_PANEL_2));
    lv_style_set_bg_opa(&s_nav, LV_OPA_COVER);
    lv_style_set_radius(&s_nav, 7);
    lv_style_set_border_width(&s_nav, 0);
    lv_style_set_text_color(&s_nav, lv_color_hex(C_MUTED));
    lv_style_set_text_font(&s_nav, &lv_font_montserrat_12);

    lv_style_init(&s_nav_active);
    lv_style_set_bg_color(&s_nav_active, lv_color_hex(0x155e75));
    lv_style_set_bg_opa(&s_nav_active, LV_OPA_COVER);
    lv_style_set_text_color(&s_nav_active, lv_color_hex(C_TEXT));

    lv_style_init(&s_btn);
    lv_style_set_bg_color(&s_btn, lv_color_hex(C_PANEL_2));
    lv_style_set_bg_opa(&s_btn, LV_OPA_COVER);
    lv_style_set_radius(&s_btn, 7);
    lv_style_set_border_width(&s_btn, 0);
    lv_style_set_shadow_width(&s_btn, 0);
    lv_style_set_text_color(&s_btn, lv_color_hex(C_TEXT));
    lv_style_set_text_font(&s_btn, &lv_font_montserrat_14);

    lv_style_init(&s_btn_primary);
    lv_style_set_bg_color(&s_btn_primary, lv_color_hex(0x155e75));
    lv_style_set_bg_opa(&s_btn_primary, LV_OPA_COVER);

    lv_style_init(&s_btn_danger);
    lv_style_set_bg_color(&s_btn_danger, lv_color_hex(0x7f1d1d));
    lv_style_set_bg_opa(&s_btn_danger, LV_OPA_COVER);

    style_text(&s_title, C_TEXT, &lv_font_montserrat_18);
    style_text(&s_body, C_TEXT, &lv_font_montserrat_14);
    style_text(&s_small, C_MUTED, &lv_font_montserrat_12);
    style_text(&s_value, C_TEXT, &lv_font_montserrat_28);

    s_styles_ready = true;
}

static lv_obj_t *make_obj(lv_obj_t *parent, const lv_style_t *style, int32_t x, int32_t y, int32_t w, int32_t h)
{
    lv_obj_t *obj = lv_obj_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_add_style(obj, style, 0);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, w, h);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    return obj;
}

static lv_obj_t *make_label(lv_obj_t *parent, const char *text, const lv_style_t *style, int32_t x, int32_t y)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_obj_remove_style_all(label);
    lv_obj_add_style(label, style, 0);
    lv_label_set_text(label, text);
    lv_obj_set_pos(label, x, y);
    return label;
}

static lv_obj_t *make_centered_label(lv_obj_t *parent, const char *text, const lv_style_t *style, int32_t y)
{
    lv_obj_t *label = make_label(parent, text, style, 0, y);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, y);
    return label;
}

static lv_obj_t *make_button(lv_obj_t *parent, const char *text, int32_t x, int32_t y, int32_t w, int32_t h, const lv_style_t *accent)
{
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_remove_style_all(btn);
    lv_obj_add_style(btn, &s_btn, 0);
    if (accent != NULL) {
        lv_obj_add_style(btn, accent, 0);
    }
    lv_obj_set_pos(btn, x, y);
    lv_obj_set_size(btn, w, h);
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *label = lv_label_create(btn);
    lv_obj_remove_style_all(label);
    lv_obj_add_style(label, &s_body, 0);
    lv_label_set_text(label, text);
    lv_obj_center(label);
    return btn;
}

static void make_topbar(lv_obj_t *screen)
{
    lv_obj_t *bar = make_obj(screen, &s_topbar, 0, 0, 800, 52);
    lv_obj_t *logo = make_obj(bar, &s_card_inset, 14, 10, 32, 32);
    lv_obj_set_style_bg_color(logo, lv_color_hex(C_TEAL), 0);
    lv_obj_t *logo_label = make_label(logo, "B", &s_body, 0, 0);
    lv_obj_center(logo_label);
    make_label(bar, "P1S Control Panel", &s_title, 58, 16);

    lv_obj_t *pill = make_obj(bar, &s_card_inset, 354, 12, 120, 28);
    lv_obj_set_style_radius(pill, 14, 0);
    lv_obj_set_style_bg_color(pill, lv_color_hex(C_AMBER), 0);
    lv_obj_t *pill_label = make_label(pill, "Printing 42%", &s_small, 0, 0);
    lv_obj_set_style_text_color(pill_label, lv_color_hex(C_TOP), 0);
    lv_obj_center(pill_label);

    make_label(bar, "LAN", &s_small, 600, 20);
    make_label(bar, "AMS 2", &s_small, 650, 20);
    make_label(bar, "18:32", &s_small, 715, 20);
}

static void make_nav(lv_obj_t *screen)
{
    lv_obj_t *rail = make_obj(screen, &s_rail, 0, 52, 88, 428);
    const char *items[] = {"Home", "Files", "Control", "AMS", "Maint", "Set"};
    for (size_t i = 0; i < 6; ++i) {
        lv_obj_t *item = make_obj(rail, i == 0 ? &s_nav_active : &s_nav, 10, (int32_t)(14 + i * 67), 68, 54);
        lv_obj_t *label = make_label(item, items[i], &s_small, 0, 0);
        lv_obj_center(label);
    }
}

static void make_job_card(lv_obj_t *screen)
{
    lv_obj_t *card = make_obj(screen, &s_card, 108, 66, 438, 320);
    make_label(card, "Dragon_Box_0.20mm", &s_title, 16, 14);
    make_label(card, "Layer 0.20 | PLA | printed 1h42m", &s_small, 16, 42);

    lv_obj_t *time = make_obj(card, &s_card_inset, 330, 12, 76, 54);
    lv_obj_set_style_pad_all(time, 0, 0);
    make_centered_label(time, "2h18m", &s_body, 9);
    make_centered_label(time, "left", &s_small, 30);

    lv_obj_t *preview = make_obj(card, &s_card_inset, 16, 76, 390, 170);
    lv_obj_set_style_bg_color(preview, lv_color_hex(0x17243a), 0);
    make_label(preview, "42%", &s_value, 276, 62);
    for (int32_t i = 0; i < 6; ++i) {
        lv_obj_t *line = make_obj(preview, &s_card_inset, 28, 28 + i * 20, 170 - i * 18, 5);
        lv_obj_set_style_bg_color(line, lv_color_hex(0x3b4b66), 0);
        lv_obj_set_style_radius(line, 2, 0);
    }

    lv_obj_t *bar = make_obj(card, &s_card_inset, 16, 276, 270, 10);
    lv_obj_set_style_radius(bar, 5, 0);
    lv_obj_t *fill = make_obj(bar, &s_card_inset, 0, 0, 113, 10);
    lv_obj_set_style_bg_color(fill, lv_color_hex(C_TEAL_2), 0);
    lv_obj_set_style_radius(fill, 5, 0);
    make_label(card, "Layer 128 / 304", &s_small, 304, 273);
}

static void make_side_stack(lv_obj_t *screen)
{
    lv_obj_t *temps = make_obj(screen, &s_card, 562, 66, 214, 140);
    lv_obj_set_style_pad_all(temps, 0, 0);
    make_label(temps, "Temperatures", &s_body, 18, 14);

    lv_obj_t *nozzle = make_obj(temps, &s_card_inset, 16, 48, 82, 76);
    lv_obj_set_style_pad_all(nozzle, 0, 0);
    make_centered_label(nozzle, "Nozzle", &s_small, 6);
    lv_obj_t *noz_value = make_centered_label(nozzle, "220°", &s_value, 24);
    lv_obj_set_style_text_color(noz_value, lv_color_hex(C_TEAL_2), 0);
    make_centered_label(nozzle, "Target 220", &s_small, 59);

    lv_obj_t *bed = make_obj(temps, &s_card_inset, 116, 48, 82, 76);
    lv_obj_set_style_pad_all(bed, 0, 0);
    make_centered_label(bed, "Bed", &s_small, 6);
    lv_obj_t *bed_value = make_centered_label(bed, "55°", &s_value, 24);
    lv_obj_set_style_text_color(bed_value, lv_color_hex(C_AMBER), 0);
    make_centered_label(bed, "Target 55", &s_small, 59);

    lv_obj_t *ams = make_obj(screen, &s_card, 562, 216, 214, 170);
    lv_obj_set_style_pad_all(ams, 0, 0);
    make_label(ams, "AMS Slots", &s_body, 18, 18);
    make_label(ams, "1A active", &s_small, 134, 21);
    const uint32_t colors[] = {0xef4444, 0xfacc15, 0x22c55e, 0x38bdf8, 0x111827, 0xf8fafc, 0xa855f7, 0x243148};
    for (size_t i = 0; i < 8; ++i) {
        lv_obj_t *slot = make_obj(ams, &s_card_inset, (int32_t)(22 + (i % 4) * 43), (int32_t)(56 + (i / 4) * 58), 36, 36);
        lv_obj_set_style_pad_all(slot, 0, 0);
        lv_obj_set_style_bg_color(slot, lv_color_hex(colors[i]), 0);
        lv_obj_set_style_radius(slot, 18, 0);
        lv_obj_set_style_border_color(slot, lv_color_hex(0xcbd5e1), 0);
        lv_obj_set_style_border_width(slot, i == 0 ? 3 : 2, 0);
        lv_obj_t *hub = make_obj(slot, &s_card_inset, 0, 0, 14, 14);
        lv_obj_set_style_pad_all(hub, 0, 0);
        lv_obj_set_style_radius(hub, 7, 0);
        lv_obj_set_style_bg_color(hub, lv_color_hex(0x07111f), 0);
        lv_obj_set_style_bg_opa(hub, LV_OPA_50, 0);
        lv_obj_center(hub);
        if (i == 0) {
            lv_obj_set_style_outline_color(slot, lv_color_hex(C_TEAL_2), 0);
            lv_obj_set_style_outline_width(slot, 2, 0);
            lv_obj_set_style_outline_pad(slot, 2, 0);
        }
        if (i == 7) {
            lv_obj_set_style_border_color(slot, lv_color_hex(C_DIM), 0);
        }
    }
}

static void make_actions(lv_obj_t *screen)
{
    const char *actions[] = {"Pause", "Speed", "Fan", "Light", "Stop"};
    for (size_t i = 0; i < 5; ++i) {
        const lv_style_t *accent = i == 0 ? &s_btn_primary : i == 4 ? &s_btn_danger : NULL;
        make_button(screen, actions[i], (int32_t)(108 + i * 133), 410, 120, 54, accent);
    }
}

void bambu_lvgl_ui_show_home(void)
{
    init_styles();

    lv_obj_t *screen = lv_obj_create(NULL);
    lv_obj_remove_style_all(screen);
    lv_obj_add_style(screen, &s_screen, 0);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

    make_topbar(screen);
    make_nav(screen);
    make_job_card(screen);
    make_side_stack(screen);
    make_actions(screen);

    lv_scr_load(screen);
}
