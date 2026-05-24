#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "bambu_panel_ui.h"

static void test_home_scene_contains_main_screen_regions(void)
{
    bambu_panel_ui_command_t commands[80];
    const size_t count = bambu_panel_ui_home_scene(commands, 80);

    assert(count > 30);
    assert(commands[0].type == BAMBU_PANEL_UI_CMD_RECT);
    assert(commands[0].x == 0);
    assert(commands[0].y == 0);
    assert(commands[0].w == 800);
    assert(commands[0].h == 480);
    assert(commands[0].color == 0x0842);

    bool has_nav = false;
    bool has_progress = false;
    bool has_ams_slot = false;
    bool has_stop_button = false;

    for (size_t i = 0; i < count; ++i) {
        if (commands[i].type == BAMBU_PANEL_UI_CMD_RECT && commands[i].x == 0 && commands[i].w == 88) {
            has_nav = true;
        }
        if (commands[i].type == BAMBU_PANEL_UI_CMD_TEXT && strcmp(commands[i].text, "42%") == 0) {
            has_progress = true;
        }
        if (commands[i].type == BAMBU_PANEL_UI_CMD_RECT && commands[i].w == 46 && commands[i].h == 34) {
            has_ams_slot = true;
        }
        if (commands[i].type == BAMBU_PANEL_UI_CMD_TEXT && strcmp(commands[i].text, "STOP") == 0) {
            has_stop_button = true;
        }
    }

    assert(has_nav);
    assert(has_progress);
    assert(has_ams_slot);
    assert(has_stop_button);
}

static void test_home_hit_testing_maps_buttons(void)
{
    assert(bambu_panel_ui_hit_test_home(622, 304) == BAMBU_PANEL_UI_HIT_PAUSE);
    assert(bambu_panel_ui_hit_test_home(716, 304) == BAMBU_PANEL_UI_HIT_LIGHT);
    assert(bambu_panel_ui_hit_test_home(622, 370) == BAMBU_PANEL_UI_HIT_FAN);
    assert(bambu_panel_ui_hit_test_home(716, 370) == BAMBU_PANEL_UI_HIT_STOP);
    assert(bambu_panel_ui_hit_test_home(130, 90) == BAMBU_PANEL_UI_HIT_NONE);
}

static void test_control_state_changes_from_button_hits(void)
{
    bambu_panel_ui_state_t state = bambu_panel_ui_state_default();

    assert(!state.paused);
    assert(!state.chamber_light_on);
    assert(state.part_fan_percent == 70);
    assert(!state.stop_confirm_pending);

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

    event = bambu_panel_ui_touch_tracker_update(&tracker, true, 622, 304);
    assert(event.type == BAMBU_PANEL_UI_TOUCH_EVENT_PRESS);
    assert(event.hit == BAMBU_PANEL_UI_HIT_PAUSE);
    assert(event.x == 622);
    assert(event.y == 304);

    event = bambu_panel_ui_touch_tracker_update(&tracker, true, 623, 305);
    assert(event.type == BAMBU_PANEL_UI_TOUCH_EVENT_NONE);

    event = bambu_panel_ui_touch_tracker_update(&tracker, false, 0, 0);
    assert(event.type == BAMBU_PANEL_UI_TOUCH_EVENT_NONE);

    event = bambu_panel_ui_touch_tracker_update(&tracker, false, 0, 0);
    assert(event.type == BAMBU_PANEL_UI_TOUCH_EVENT_RELEASE);
    assert(event.hit == BAMBU_PANEL_UI_HIT_PAUSE);
    assert(event.x == 622);
    assert(event.y == 304);

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
}

int main(void)
{
    test_home_scene_contains_main_screen_regions();
    test_home_hit_testing_maps_buttons();
    test_control_state_changes_from_button_hits();
    test_status_text_describes_current_state();
    test_touch_tracker_emits_one_event_per_press_and_release();
    test_control_action_describes_button_intent();
    return 0;
}
