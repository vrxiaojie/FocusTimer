#include <stdio.h>
#include <sys/lock.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "lvgl_user.h"
#include "screens.h"
#include "pomodoro_screen_calls.h"

#define POMODORO_SCREEN_UPDATE_MS 1000

static const char *TAG = "pomodoro_screen";
static TaskHandle_t s_pomodoro_screen_update_task_handle = NULL;

// 番茄钟状态
typedef enum
{
    POMODORO_STATE_FOCUS, // 专注状态
    POMODORO_STATE_REST   // 休息状态
} pomodoro_state_t;

// 全局变量
static pomodoro_state_t s_pomodoro_state = POMODORO_STATE_FOCUS;
static bool s_is_paused = true;
static uint16_t s_remaining_seconds = 25 * 60; // 默认25分钟
static uint8_t s_focus_count = 0;
static uint8_t s_nap_count = 0;

// 用于标记状态是否变化，需要更新UI
static volatile bool s_state_changed = true;

// 默认时间设置
static const uint8_t FOCUS_TIME_MINUTES = 25;
static const uint8_t REST_TIME_MINUTES = 5;

static void update_pomodoro_labels(void)
{
    char minute_text[10];
    char second_text[10];
    char status_text[16];
    char focus_count_text[8];
    char nap_count_text[8];

    int minutes = s_remaining_seconds / 60;
    int seconds = s_remaining_seconds % 60;

    (void)snprintf(minute_text, sizeof(minute_text), "%02d", minutes);
    (void)snprintf(second_text, sizeof(second_text), "%02d", seconds);
    (void)snprintf(status_text, sizeof(status_text), "%s", s_pomodoro_state == POMODORO_STATE_FOCUS ? "专注中" : "休息中");
    (void)snprintf(focus_count_text, sizeof(focus_count_text), "%02d", s_focus_count);
    (void)snprintf(nap_count_text, sizeof(nap_count_text), "%02d", s_nap_count);

    _lock_acquire(&lvgl_api_lock);

    if (objects.pomodoro_scr_time_minute_label != NULL)
    {
        lv_label_set_text(objects.pomodoro_scr_time_minute_label, minute_text);
    }
    if (objects.main_scr_time_second_label != NULL)
    {
        lv_label_set_text(objects.main_scr_time_second_label, second_text);
    }
    if (objects.pomodoro_scr_working_status_label != NULL)
    {
        lv_label_set_text(objects.pomodoro_scr_working_status_label, status_text);
    }
    if (objects.pomodoro_scr_focus_cnt != NULL)
    {
        lv_label_set_text(objects.pomodoro_scr_focus_cnt, focus_count_text);
    }
    if (objects.pomodoro_scr_nap_cnt != NULL)
    {
        lv_label_set_text(objects.pomodoro_scr_nap_cnt, nap_count_text);
    }

    // 更新开始/暂停按钮的图标
    if (objects.pomodoro_scr_start_pause_btn != NULL)
    {
        lv_obj_t *btn_label = lv_obj_get_child(objects.pomodoro_scr_start_pause_btn, 0);
        if (btn_label != NULL)
        {
            if (s_is_paused)
            {
                lv_label_set_text(btn_label, ""); // 播放图标
                lv_obj_set_pos(btn_label, -13, -4);
            }
            else
            {
                lv_label_set_text(btn_label, ""); // 暂停图标
                lv_obj_set_pos(btn_label, -7, -5);
            }
        }
    }

    _lock_release(&lvgl_api_lock);

    s_state_changed = false;
}

static void pomodoro_screen_update_task(void *arg)
{
    (void)arg;

    // 延迟启动，等待屏幕动画完成
    vTaskDelay(pdMS_TO_TICKS(300));

    // 首次更新显示
    update_pomodoro_labels();

    TickType_t last_wake_time = xTaskGetTickCount();

    while (1)
    {
        bool need_update = false;

        if (!s_is_paused)
        {
            if (s_remaining_seconds > 0)
            {
                s_remaining_seconds--;
            }
            else
            {
                // 倒计时结束，切换状态
                if (s_pomodoro_state == POMODORO_STATE_FOCUS)
                {
                    s_focus_count++;
                    s_pomodoro_state = POMODORO_STATE_REST;
                    s_remaining_seconds = REST_TIME_MINUTES * 60;
                }
                else
                {
                    s_nap_count++;
                    s_pomodoro_state = POMODORO_STATE_FOCUS;
                    s_remaining_seconds = FOCUS_TIME_MINUTES * 60;
                }
            }
            need_update = true;
        }

        // 如果状态有变化或者时间在走，更新UI
        if (need_update || s_state_changed)
        {
            update_pomodoro_labels();
        }

        xTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(POMODORO_SCREEN_UPDATE_MS));
    }
}

void pomodoro_screen_start_update_task(void)
{
    if (s_pomodoro_screen_update_task_handle != NULL)
    {
        return;
    }

    xTaskCreate(pomodoro_screen_update_task,
                "pomodoro_screen_update",
                4096,
                NULL,
                5,
                &s_pomodoro_screen_update_task_handle);
    ESP_LOGI(TAG, "Pomodoro screen update task started");
}

void pomodoro_screen_stop_update_task(void)
{
    if (s_pomodoro_screen_update_task_handle != NULL)
    {
        vTaskDelete(s_pomodoro_screen_update_task_handle);
        s_pomodoro_screen_update_task_handle = NULL;
        ESP_LOGI(TAG, "Pomodoro screen update task stopped");
    }
}

void pomodoro_screen_toggle_pause(void)
{
    s_is_paused = !s_is_paused;
    s_state_changed = true;
    ESP_LOGI(TAG, "Pomodoro %s", s_is_paused ? "paused" : "resumed");
}

void pomodoro_screen_reset(void)
{
    s_is_paused = true;
    if (s_pomodoro_state == POMODORO_STATE_FOCUS)
    {
        s_remaining_seconds = FOCUS_TIME_MINUTES * 60;
    }
    else
    {
        s_remaining_seconds = REST_TIME_MINUTES * 60;
    }
    s_state_changed = true;
    ESP_LOGI(TAG, "Pomodoro reset");
}

void pomodoro_screen_skip(void)
{
    // 切换状态
    if (s_pomodoro_state == POMODORO_STATE_FOCUS)
    {
        s_focus_count++;
        s_pomodoro_state = POMODORO_STATE_REST;
        s_remaining_seconds = REST_TIME_MINUTES * 60;
    }
    else
    {
        s_nap_count++;
        s_pomodoro_state = POMODORO_STATE_FOCUS;
        s_remaining_seconds = FOCUS_TIME_MINUTES * 60;
    }
    s_is_paused = true;
    s_state_changed = true;
    ESP_LOGI(TAG, "Pomodoro skipped to next state");
}
