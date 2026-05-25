#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "bambu_panel_ui.h"

static bool scene_has_text(const bambu_panel_ui_command_t *commands, size_t count, const char *text)
{
    for (size_t i = 0; i < count; ++i) {
        if (commands[i].type == BAMBU_PANEL_UI_CMD_TEXT && strcmp(commands[i].text, text) == 0) {
            return true;
        }
    }
    return false;
}

static bool scene_has_rect(const bambu_panel_ui_command_t *commands,
                           size_t count,
                           uint16_t x,
                           uint16_t y,
                           uint16_t w,
                           uint16_t h)
{
    for (size_t i = 0; i < count; ++i) {
        if (commands[i].type == BAMBU_PANEL_UI_CMD_RECT && commands[i].x == x && commands[i].y == y &&
            commands[i].w == w && commands[i].h == h) {
            return true;
        }
    }
    return false;
}

static void test_home_scene_contains_main_screen_regions(void)
{
    bambu_panel_ui_command_t commands[128];
    const size_t count = bambu_panel_ui_home_scene(commands, 128);

    assert(count > 55);
    assert(commands[0].type == BAMBU_PANEL_UI_CMD_RECT);
    assert(commands[0].x == 0);
    assert(commands[0].y == 0);
    assert(commands[0].w == 800);
    assert(commands[0].h == 480);
    assert(commands[0].color == 0x0842);

    assert(scene_has_rect(commands, count, 0, 0, 800, 52));
    assert(scene_has_rect(commands, count, 0, 52, 88, 428));
    assert(scene_has_rect(commands, count, 108, 66, 438, 216));
    assert(scene_has_rect(commands, count, 562, 66, 214, 106));
    assert(scene_has_rect(commands, count, 562, 190, 214, 92));
    assert(scene_has_rect(commands, count, 108, 306, 120, 54));
    assert(scene_has_rect(commands, count, 640, 306, 120, 54));

    assert(scene_has_text(commands, count, "P1S PANEL"));
    assert(scene_has_text(commands, count, "PRINTING 42%"));
    assert(scene_has_text(commands, count, "DRAGON_BOX_020"));
    assert(scene_has_text(commands, count, "2H18M"));
    assert(scene_has_text(commands, count, "42%"));
    assert(scene_has_text(commands, count, "L128/304"));
    assert(scene_has_text(commands, count, "NOZZLE"));
    assert(scene_has_text(commands, count, "220C"));
    assert(scene_has_text(commands, count, "BED"));
    assert(scene_has_text(commands, count, "55C"));
    assert(scene_has_text(commands, count, "AMS SLOTS"));
    assert(scene_has_text(commands, count, "1A ACTIVE"));
    assert(scene_has_text(commands, count, "PAUSE"));
    assert(scene_has_text(commands, count, "SPEED"));
    assert(scene_has_text(commands, count, "FAN"));
    assert(scene_has_text(commands, count, "LIGHT"));
    assert(scene_has_text(commands, count, "STOP"));
}

static void test_home_hit_testing_maps_buttons(void)
{
    assert(bambu_panel_ui_hit_test_home(44, 89) == BAMBU_PANEL_UI_HIT_NAV_HOME);
    assert(bambu_panel_ui_hit_test_home(44, 149) == BAMBU_PANEL_UI_HIT_NAV_FILES);
    assert(bambu_panel_ui_hit_test_home(44, 209) == BAMBU_PANEL_UI_HIT_NAV_CTRL);
    assert(bambu_panel_ui_hit_test_home(44, 269) == BAMBU_PANEL_UI_HIT_NAV_AMS);
    assert(bambu_panel_ui_hit_test_home(44, 329) == BAMBU_PANEL_UI_HIT_NAV_MAINT);
    assert(bambu_panel_ui_hit_test_home(44, 389) == BAMBU_PANEL_UI_HIT_NAV_SET);
    assert(bambu_panel_ui_hit_test_home(620, 118) == BAMBU_PANEL_UI_HIT_NAV_CTRL);
    assert(bambu_panel_ui_hit_test_home(168, 333) == BAMBU_PANEL_UI_HIT_PAUSE);
    assert(bambu_panel_ui_hit_test_home(301, 333) == BAMBU_PANEL_UI_HIT_NAV_CTRL);
    assert(bambu_panel_ui_hit_test_home(434, 333) == BAMBU_PANEL_UI_HIT_FAN);
    assert(bambu_panel_ui_hit_test_home(567, 333) == BAMBU_PANEL_UI_HIT_LIGHT);
    assert(bambu_panel_ui_hit_test_home(700, 333) == BAMBU_PANEL_UI_HIT_STOP);
    assert(bambu_panel_ui_hit_test_home(130, 90) == BAMBU_PANEL_UI_HIT_NONE);
}

static void test_page_scene_changes_with_current_page(void)
{
    bambu_panel_ui_command_t commands[96];
    bambu_panel_ui_state_t state = bambu_panel_ui_state_default();
    size_t count = 0;

    assert(state.current_page == BAMBU_PANEL_UI_PAGE_HOME);
    count = bambu_panel_ui_page_scene(&state, commands, 96);
    assert(count > 30);
    assert(scene_has_text(commands, count, "PRINTING 42%"));

    state.current_page = BAMBU_PANEL_UI_PAGE_CTRL;
    count = bambu_panel_ui_page_scene(&state, commands, 96);
    assert(scene_has_text(commands, count, "CONTROL"));
    assert(scene_has_text(commands, count, "NOZZLE"));
    assert(scene_has_text(commands, count, "BED"));
    assert(scene_has_text(commands, count, "FAN"));
    assert(scene_has_text(commands, count, "SPEED"));
    assert(scene_has_text(commands, count, "FLOW"));
    assert(scene_has_text(commands, count, "PREHEAT"));
    assert(scene_has_text(commands, count, "PLA"));
    assert(scene_has_text(commands, count, "PETG"));
    assert(scene_has_text(commands, count, "ABS"));
    assert(scene_has_text(commands, count, "CUSTOM"));
    assert(scene_has_rect(commands, count, 108, 58, 210, 150));
    assert(scene_has_rect(commands, count, 337, 58, 210, 150));
    assert(scene_has_rect(commands, count, 566, 58, 210, 150));
    assert(scene_has_rect(commands, count, 108, 230, 210, 130));
    assert(scene_has_rect(commands, count, 337, 230, 210, 130));
    assert(scene_has_rect(commands, count, 566, 230, 210, 130));

    state.current_page = BAMBU_PANEL_UI_PAGE_AMS;
    count = bambu_panel_ui_page_scene(&state, commands, 96);
    assert(scene_has_text(commands, count, "AMS STATUS"));

    state.current_page = BAMBU_PANEL_UI_PAGE_MAINT;
    count = bambu_panel_ui_page_scene(&state, commands, 96);
    assert(scene_has_text(commands, count, "MAINTENANCE"));

    state.current_page = BAMBU_PANEL_UI_PAGE_SET;
    count = bambu_panel_ui_page_scene(&state, commands, 96);
    assert(scene_has_text(commands, count, "SETTINGS"));
}

static void test_control_state_changes_from_button_hits(void)
{
    bambu_panel_ui_state_t state = bambu_panel_ui_state_default();

    assert(!state.paused);
    assert(!state.chamber_light_on);
    assert(state.part_fan_percent == 70);
    assert(!state.stop_confirm_pending);
    assert(state.current_page == BAMBU_PANEL_UI_PAGE_HOME);

    bambu_panel_ui_apply_hit(&state, BAMBU_PANEL_UI_HIT_PAUSE);
    assert(state.paused);
    bambu_panel_ui_apply_hit(&state, BAMBU_PANEL_UI_HIT_PAUSE);
    assert(!state.paused);

    bambu_panel_ui_apply_hit(&state, BAMBU_PANEL_UI_HIT_LIGHT);
    assert(state.chamber_light_on);

    bambu_panel_ui_apply_hit(&state, BAMBU_PANEL_UI_HIT_FAN);
    assert(state.part_fan_percent == 100);
    bambu_panel_ui_apply_hit(&state, BAMBU_PANEL_UI_HIT_FAN);
    assert(state.part_fan_percent == 0);
    bambu_panel_ui_apply_hit(&state, BAMBU_PANEL_UI_HIT_FAN);
    assert(state.part_fan_percent == 70);

    bambu_panel_ui_apply_hit(&state, BAMBU_PANEL_UI_HIT_STOP);
    assert(state.stop_confirm_pending);
    bambu_panel_ui_apply_hit(&state, BAMBU_PANEL_UI_HIT_NONE);
    assert(state.stop_confirm_pending);
    bambu_panel_ui_apply_hit(&state, BAMBU_PANEL_UI_HIT_LIGHT);
    assert(!state.stop_confirm_pending);
    assert(state.current_page == BAMBU_PANEL_UI_PAGE_HOME);
}

static void test_status_text_describes_current_state(void)
{
    char text[32];
    bambu_panel_ui_state_t state = bambu_panel_ui_state_default();

    bambu_panel_ui_status_text(&state, BAMBU_PANEL_UI_HIT_NONE, text, sizeof(text));
    assert(strcmp(text, "RUN LAMP OFF FAN 70") == 0);

    bambu_panel_ui_apply_hit(&state, BAMBU_PANEL_UI_HIT_PAUSE);
    bambu_panel_ui_apply_hit(&state, BAMBU_PANEL_UI_HIT_LIGHT);
    bambu_panel_ui_apply_hit(&state, BAMBU_PANEL_UI_HIT_FAN);
    bambu_panel_ui_status_text(&state, BAMBU_PANEL_UI_HIT_FAN, text, sizeof(text));
    assert(strcmp(text, "PAUSED LAMP ON FAN 100") == 0);

    bambu_panel_ui_apply_hit(&state, BAMBU_PANEL_UI_HIT_STOP);
    bambu_panel_ui_status_text(&state, BAMBU_PANEL_UI_HIT_STOP, text, sizeof(text));
    assert(strcmp(text, "STOP CONFIRM") == 0);
}

static void test_touch_tracker_emits_one_event_per_press_and_release(void)
{
    bambu_panel_ui_touch_tracker_t tracker = bambu_panel_ui_touch_tracker_default();
    bambu_panel_ui_touch_event_t event = {0};

    event = bambu_panel_ui_touch_tracker_update(&tracker, false, 0, 0);
    assert(event.type == BAMBU_PANEL_UI_TOUCH_EVENT_NONE);

    event = bambu_panel_ui_touch_tracker_update(&tracker, true, 168, 333);
    assert(event.type == BAMBU_PANEL_UI_TOUCH_EVENT_PRESS);
    assert(event.hit == BAMBU_PANEL_UI_HIT_PAUSE);
    assert(event.x == 168);
    assert(event.y == 333);

    event = bambu_panel_ui_touch_tracker_update(&tracker, true, 169, 334);
    assert(event.type == BAMBU_PANEL_UI_TOUCH_EVENT_NONE);

    event = bambu_panel_ui_touch_tracker_update(&tracker, false, 0, 0);
    assert(event.type == BAMBU_PANEL_UI_TOUCH_EVENT_NONE);

    event = bambu_panel_ui_touch_tracker_update(&tracker, false, 0, 0);
    assert(event.type == BAMBU_PANEL_UI_TOUCH_EVENT_RELEASE);
    assert(event.hit == BAMBU_PANEL_UI_HIT_PAUSE);
    assert(event.x == 168);
    assert(event.y == 333);

    event = bambu_panel_ui_touch_tracker_update(&tracker, false, 0, 0);
    assert(event.type == BAMBU_PANEL_UI_TOUCH_EVENT_NONE);
}

static void test_control_action_describes_button_intent(void)
{
    bambu_panel_ui_state_t state = bambu_panel_ui_state_default();
    bambu_panel_ui_action_t action = {0};

    action = bambu_panel_ui_apply_hit_with_action(&state, BAMBU_PANEL_UI_HIT_PAUSE);
    assert(action.type == BAMBU_PANEL_UI_ACTION_PAUSE_PRINT);
    assert(action.value == 0);
    assert(state.paused);

    action = bambu_panel_ui_apply_hit_with_action(&state, BAMBU_PANEL_UI_HIT_PAUSE);
    assert(action.type == BAMBU_PANEL_UI_ACTION_RESUME_PRINT);
    assert(!state.paused);

    action = bambu_panel_ui_apply_hit_with_action(&state, BAMBU_PANEL_UI_HIT_LIGHT);
    assert(action.type == BAMBU_PANEL_UI_ACTION_SET_CHAMBER_LIGHT);
    assert(action.value == 1);
    assert(state.chamber_light_on);

    action = bambu_panel_ui_apply_hit_with_action(&state, BAMBU_PANEL_UI_HIT_LIGHT);
    assert(action.type == BAMBU_PANEL_UI_ACTION_SET_CHAMBER_LIGHT);
    assert(action.value == 0);
    assert(!state.chamber_light_on);

    action = bambu_panel_ui_apply_hit_with_action(&state, BAMBU_PANEL_UI_HIT_FAN);
    assert(action.type == BAMBU_PANEL_UI_ACTION_SET_PART_FAN);
    assert(action.value == 100);
    assert(state.part_fan_percent == 100);

    action = bambu_panel_ui_apply_hit_with_action(&state, BAMBU_PANEL_UI_HIT_STOP);
    assert(action.type == BAMBU_PANEL_UI_ACTION_REQUEST_STOP_CONFIRMATION);
    assert(state.stop_confirm_pending);

    action = bambu_panel_ui_apply_hit_with_action(&state, BAMBU_PANEL_UI_HIT_NONE);
    assert(action.type == BAMBU_PANEL_UI_ACTION_NONE);
    assert(state.stop_confirm_pending);

    action = bambu_panel_ui_apply_hit_with_action(&state, BAMBU_PANEL_UI_HIT_NAV_CTRL);
    assert(action.type == BAMBU_PANEL_UI_ACTION_SWITCH_PAGE);
    assert(action.value == BAMBU_PANEL_UI_PAGE_CTRL);
    assert(state.current_page == BAMBU_PANEL_UI_PAGE_CTRL);

    action = bambu_panel_ui_apply_hit_with_action(&state, BAMBU_PANEL_UI_HIT_NAV_AMS);
    assert(action.type == BAMBU_PANEL_UI_ACTION_SWITCH_PAGE);
    assert(action.value == BAMBU_PANEL_UI_PAGE_AMS);
    assert(state.current_page == BAMBU_PANEL_UI_PAGE_AMS);

    action = bambu_panel_ui_apply_hit_with_action(&state, BAMBU_PANEL_UI_HIT_NAV_MAINT);
    assert(action.type == BAMBU_PANEL_UI_ACTION_SWITCH_PAGE);
    assert(action.value == BAMBU_PANEL_UI_PAGE_MAINT);
    assert(state.current_page == BAMBU_PANEL_UI_PAGE_MAINT);
}

int main(void)
{
    test_home_scene_contains_main_screen_regions();
    test_home_hit_testing_maps_buttons();
    test_page_scene_changes_with_current_page();
    test_control_state_changes_from_button_hits();
    test_status_text_describes_current_state();
    test_touch_tracker_emits_one_event_per_press_and_release();
    test_control_action_describes_button_intent();
    return 0;
}
