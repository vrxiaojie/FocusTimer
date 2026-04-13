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

extern esp_lcd_panel_handle_t panel_handle;

void action_btn1_action(lv_event_t *e)
{
    static bool rotate = false;
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED)
    {
        lv_display_set_rotation(lvgl_display, rotate ? LV_DISPLAY_ROTATION_270 : LV_DISPLAY_ROTATION_90);
        rotate = !rotate;
    }
}

void action_btn2_action(lv_event_t *e)
{
    static bool invert_color = false;
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED)
    {
        ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, invert_color)); // 调整反色
        invert_color = !invert_color;
    }
}


void action_btn3_action(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED)
    {
        
    }
}

// 切换电源模式
void action_btn4_action(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED)
    {
        switch (esp_lcd_panel_st7305_get_power_mode(panel_handle))
        {
        case ST7305_PWR_MODE_HPM:
            esp_lcd_panel_st7305_set_power_mode(panel_handle, ST7305_PWR_MODE_LPM);
            break;
        case ST7305_PWR_MODE_LPM:
            esp_lcd_panel_st7305_set_power_mode(panel_handle, ST7305_PWR_MODE_HPM);
            break;
        }
    }
}

void action_btn5_action(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    static char buf[8] = {0};
    static uint8_t cnt = 0;
    esp_pm_config_t pm_config = {.light_sleep_enable = false, .min_freq_mhz = 48};

    if (code == LV_EVENT_SHORT_CLICKED)
    {
        switch (cnt)
        {
        case 0:
            snprintf(buf, sizeof(buf), "48MHz");
            pm_config.min_freq_mhz = 48;
            pm_config.max_freq_mhz = 48;
            break;
        case 1:
            snprintf(buf, sizeof(buf), "64MHz");
            pm_config.min_freq_mhz = 64;
            pm_config.max_freq_mhz = 64;
            break;
        case 2:
            snprintf(buf, sizeof(buf), "96MHz");
            pm_config.min_freq_mhz = 96;
            pm_config.max_freq_mhz = 96;
            break;
        }
        ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
        cnt++;
        cnt %= 3;

        lv_label_set_text(lv_obj_get_child(objects.obj4, 0), buf);
    }
}

void action_btn6_action(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED)
    {
        lv_screen_load(objects.timer);
    }
}

void action_timer_back_btn(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SHORT_CLICKED)
    {
        lv_screen_load(objects.main);
    }
}

#include "esp_timer.h"
#include "esp_pm.h"

static esp_timer_handle_t timer_handle = NULL;
static int current_time_seconds = 0;

static void timer_screen_cb(void *arg)
{
    current_time_seconds++;
    int m = current_time_seconds / 60;
    int s = current_time_seconds % 60;

    char buf_m[11];
    char buf_s[11];
    snprintf(buf_m, sizeof(buf_m), "%02d", m);
    snprintf(buf_s, sizeof(buf_s), "%02d", s);

    // 如果你有LVGL的互斥锁（例如 lv_port_sem），建议在这里加锁
    _lock_acquire(&lvgl_api_lock);
    lv_label_set_text(objects.time_minute, buf_m);
    lv_label_set_text(objects.time_second, buf_s);
    _lock_release(&lvgl_api_lock);
}

void action_timer_screen_action(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SCREEN_LOAD_START)
    {
        current_time_seconds = 0;
        const esp_timer_create_args_t timer_args = {
            .callback = &timer_screen_cb,
            .arg = NULL,
            .name = "timer_screen",
            .skip_unhandled_events = true // 如果系统睡眠过头忽略掉丢失的事件
        };
        ESP_ERROR_CHECK(esp_timer_create(&timer_args, &timer_handle));
        ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handle, 1000000)); // 1秒 = 1000000微秒

        // 开启自动Light Sleep特性
        esp_pm_config_t pm_config = {
            .max_freq_mhz = 48,
            .min_freq_mhz = 48};
        ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
        esp_lcd_panel_st7305_set_power_mode(panel_handle, ST7305_PWR_MODE_LPM); // 进入低功耗模式
    }
    else if (code == LV_EVENT_SCREEN_UNLOAD_START)
    {
        if (timer_handle)
        {
            esp_timer_stop(timer_handle);
            esp_timer_delete(timer_handle);
            timer_handle = NULL;
        }

        // 退出页面
        esp_pm_config_t pm_config = {
            .max_freq_mhz = 96,
            .min_freq_mhz = 48};
        ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
        esp_lcd_panel_st7305_set_power_mode(panel_handle, ST7305_PWR_MODE_HPM);
    }
}