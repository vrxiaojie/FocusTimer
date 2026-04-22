#include "actions.h"
#include "ui.h"
#include "esp_lcd_panel_ops.h"
#include "st7305_2p9.h"
#include "esp_log.h"
#include "esp_pm.h"
#include "screens.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "lvgl_user.h"
#include "stcc4.h"
#include "main_screen_calls.h"
#include "mp3_screen_calls.h"
#include "pomodoro_screen_calls.h"
#include "message_screen_calls.h"

extern esp_lcd_panel_handle_t panel_handle;

static bool ui_action_blocked_by_message_modal(void)
{
    return message_screen_is_modal_active();
}

void action_goto_submain_btn(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED)
    {
        if (ui_action_blocked_by_message_modal())
        {
            return;
        }
        lv_screen_load_anim(objects.sub_main, LV_SCREEN_LOAD_ANIM_OVER_LEFT, 200, 0, false);
    }
}

void action_back_to_main_btn(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED)
    {
        if (ui_action_blocked_by_message_modal())
        {
            return;
        }
        lv_screen_load_anim(objects.main, LV_SCREEN_LOAD_ANIM_OVER_RIGHT, 200, 0, false);
    }
}

void action_main_scr(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SCREEN_LOADED)
    {
        main_screen_start_update_task();
    }
    if (code == LV_EVENT_SCREEN_UNLOADED)
    {
        main_screen_stop_update_task();
    }
}

void action_submain_scr_enter_pomodoro_btn(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED)
    {
        if (ui_action_blocked_by_message_modal())
        {
            return;
        }
        lv_screen_load_anim(objects.pomodoro, LV_SCREEN_LOAD_ANIM_OVER_LEFT, 200, 0, false);
    }
}

void action_submain_scr_enter_mp3_btn(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED)
    {
        if (ui_action_blocked_by_message_modal())
        {
            return;
        }
        lv_screen_load_anim(objects.mp3, LV_SCREEN_LOAD_ANIM_OVER_LEFT, 200, 0, false);
    }
}

void action_submain_scr_enter_setting_btn(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED)
    {
        if (ui_action_blocked_by_message_modal())
        {
            return;
        }
        lv_screen_load_anim(objects.setting, LV_SCREEN_LOAD_ANIM_OVER_LEFT, 200, 0, false);
    }
}

void action_pomodoro_scr_start_pause_btn(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED)
    {
        if (ui_action_blocked_by_message_modal())
        {
            return;
        }
        pomodoro_screen_toggle_pause();
    }
}

void action_pomodoro_scr_reset_btn(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED)
    {
        if (ui_action_blocked_by_message_modal())
        {
            return;
        }
        pomodoro_screen_reset();
    }
}

void action_pomodoro_scr_skip_btn(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED)
    {
        if (ui_action_blocked_by_message_modal())
        {
            return;
        }
        pomodoro_screen_skip();
    }
}

void action_pomodoro_scr(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SCREEN_LOAD_START)
    {
        pomodoro_screen_start_update_task();
    }
}

void action_message_scr_btn(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED)
    {
        message_screen_handle_ok();
    }
}

void action_mp3_scr(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SCREEN_LOADED)
    {
        mp3_screen_start_update_task();
    }
    if (code == LV_EVENT_SCREEN_UNLOADED)
    {
        mp3_screen_stop_update_task();
    }
}

void action_mp3_scr_backward_btn(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED)
    {
        if (ui_action_blocked_by_message_modal())
        {
            return;
        }
        mp3_screen_play_prev();
    }
}

void action_mp3_scr_play_pause_btn(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED)
    {
        if (ui_action_blocked_by_message_modal())
        {
            return;
        }
        mp3_screen_toggle_pause();
    }
}

void action_mp3_scr_forward_btn(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED)
    {
        if (ui_action_blocked_by_message_modal())
        {
            return;
        }
        mp3_screen_play_next();
    }
}

void action_mp3_scr_volume_slider(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_VALUE_CHANGED)
    {
        if (ui_action_blocked_by_message_modal())
        {
            return;
        }
        mp3_screen_set_volume_from_slider(lv_event_get_target(e));
    }
}