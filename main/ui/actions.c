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

extern esp_lcd_panel_handle_t panel_handle;

void action_main_scr_next_page_btn(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED)
    {
        lv_screen_load_anim(objects.sub_main, LV_SCREEN_LOAD_ANIM_OVER_LEFT, 200, 0, false);
    }
}

void action_submain_scr_prev_page_btn(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED)
    {
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
