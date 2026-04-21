#include <stdio.h>
#include <sys/lock.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_timer.h"
#include "esp_log.h"

#include "lvgl_user.h"
#include "screens.h"
#include "fonts.h"
#include "pomodoro_screen_calls.h"

#define POMODORO_SCREEN_UPDATE_MS 1000
#define POMODORO_TIMEOUT_POPUP_AUTO_CLOSE_TICKS 3

static const char *TAG = "pomodoro_screen";
static TaskHandle_t s_pomodoro_screen_update_task_handle = NULL;
static esp_timer_handle_t s_pomodoro_countdown_timer_handle = NULL;

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
static bool s_waiting_transition_confirm = false;
static uint8_t s_popup_auto_close_ticks = 0;

static lv_obj_t *s_timeout_popup = NULL;

// 默认时间设置
static const uint8_t FOCUS_TIME_MINUTES = 25;
static const uint8_t REST_TIME_MINUTES = 5;

static portMUX_TYPE s_pomodoro_lock = portMUX_INITIALIZER_UNLOCKED;

static void pomodoro_show_timeout_popup_locked(void);
static void pomodoro_hide_timeout_popup_locked(void);

static void pomodoro_notify_ui_task(void)
{
    if (s_pomodoro_screen_update_task_handle != NULL)
    {
        xTaskNotifyGive(s_pomodoro_screen_update_task_handle);
    }
}

static void pomodoro_go_to_next_stage(bool increase_counter)
{
    if (s_pomodoro_state == POMODORO_STATE_FOCUS)
    {
        if (increase_counter)
        {
            s_focus_count++;
        }
        s_pomodoro_state = POMODORO_STATE_REST;
        s_remaining_seconds = REST_TIME_MINUTES * 60;
    }
    else
    {
        if (increase_counter)
        {
            s_nap_count++;
        }
        s_pomodoro_state = POMODORO_STATE_FOCUS;
        s_remaining_seconds = FOCUS_TIME_MINUTES * 60;
    }
}

static void pomodoro_show_timeout_popup_locked(void)
{
    if (s_timeout_popup != NULL)
    {
        lv_obj_clear_flag(s_timeout_popup, LV_OBJ_FLAG_HIDDEN);
        return;
    }

    lv_obj_t *parent = objects.pomodoro != NULL ? objects.pomodoro : lv_screen_active();
    s_timeout_popup = lv_obj_create(parent);
    lv_obj_set_size(s_timeout_popup, 250, 150);
    lv_obj_center(s_timeout_popup);
    lv_obj_set_style_radius(s_timeout_popup, 12, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t *title_label = lv_label_create(s_timeout_popup);
    lv_label_set_text(title_label, "时间到");
    lv_obj_set_style_text_font(title_label, &ui_font_siyuanheiti_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 12);

    lv_obj_t *msg_label = lv_label_create(s_timeout_popup);
    char tmp[36];
    sprintf(tmp, "3秒后自动进入\n%s时间", s_pomodoro_state == POMODORO_STATE_FOCUS ? "休息" : "专注");
    lv_label_set_text(msg_label, tmp);
    lv_obj_set_width(msg_label, 250 - 20);
    lv_obj_set_style_text_align(msg_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(msg_label, &ui_font_siyuanheiti_20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(msg_label, LV_ALIGN_CENTER, 0, 20);
}

static void pomodoro_hide_timeout_popup_locked(void)
{
    if (s_timeout_popup != NULL)
    {
        lv_obj_del(s_timeout_popup);
        s_timeout_popup = NULL;
    }
}

static void update_pomodoro_labels_locked(void)
{
    char time_text[10];
    char status_text[16];
    char focus_count_text[8];
    char nap_count_text[8];
    uint16_t remaining_seconds;
    uint8_t focus_count;
    uint8_t nap_count;
    bool is_paused;
    pomodoro_state_t pomodoro_state;
    uint16_t total_seconds;
    int32_t progress_percent;

    portENTER_CRITICAL(&s_pomodoro_lock);
    remaining_seconds = s_remaining_seconds;
    focus_count = s_focus_count;
    nap_count = s_nap_count;
    is_paused = s_is_paused;
    pomodoro_state = s_pomodoro_state;
    portEXIT_CRITICAL(&s_pomodoro_lock);

    total_seconds = (pomodoro_state == POMODORO_STATE_FOCUS ? FOCUS_TIME_MINUTES : REST_TIME_MINUTES) * 60;
    progress_percent = total_seconds > 0 ? (int32_t)((remaining_seconds * 100U) / total_seconds) : 0;

    (void)snprintf(time_text, sizeof(time_text), "%02d:%02d", remaining_seconds / 60, remaining_seconds % 60);
    (void)snprintf(status_text, sizeof(status_text), "%s", pomodoro_state == POMODORO_STATE_FOCUS ? "专注中" : "休息中");
    (void)snprintf(focus_count_text, sizeof(focus_count_text), "%02d", focus_count);
    (void)snprintf(nap_count_text, sizeof(nap_count_text), "%02d", nap_count);

    if (objects.pomodoro_scr_timer_label != NULL)
    {
        lv_label_set_text(objects.pomodoro_scr_timer_label, time_text);
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
    if (objects.pomodoro_scr_arc_progress != NULL)
    {
        lv_arc_set_value(objects.pomodoro_scr_arc_progress, progress_percent);
    }

    // 更新开始/暂停按钮的图标
    if (objects.pomodoro_scr_start_pause_btn != NULL)
    {
        lv_obj_t *btn_label = lv_obj_get_child(objects.pomodoro_scr_start_pause_btn, 0);
        if (btn_label != NULL)
        {
            if (is_paused)
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
}

static void pomodoro_countdown_timer_cb(void *arg)
{
    (void)arg;
    bool should_notify_ui = false;

    portENTER_CRITICAL(&s_pomodoro_lock);
    if (!s_is_paused && !s_waiting_transition_confirm)
    {
        if (s_remaining_seconds > 0)
        {
            s_remaining_seconds--;
            should_notify_ui = true;
        }

        if (s_remaining_seconds == 0)
        {
            s_waiting_transition_confirm = true;
            s_is_paused = true;
            s_popup_auto_close_ticks = POMODORO_TIMEOUT_POPUP_AUTO_CLOSE_TICKS;
            should_notify_ui = true;
        }
    }
    else if (s_waiting_transition_confirm)
    {
        if (s_popup_auto_close_ticks > 0)
        {
            s_popup_auto_close_ticks--;
            should_notify_ui = true;
        }

        if (s_popup_auto_close_ticks == 0)
        {
            pomodoro_go_to_next_stage(true);
            s_waiting_transition_confirm = false;
            s_is_paused = true;
            should_notify_ui = true;
        }
    }
    portEXIT_CRITICAL(&s_pomodoro_lock);

    if (should_notify_ui)
    {
        pomodoro_notify_ui_task();
    }
}

static void pomodoro_screen_update_task(void *arg)
{
    (void)arg;
    pomodoro_notify_ui_task();

    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        portENTER_CRITICAL(&s_pomodoro_lock);
        bool waiting_transition_confirm = s_waiting_transition_confirm;
        portEXIT_CRITICAL(&s_pomodoro_lock);

        _lock_acquire(&lvgl_api_lock);
        if (waiting_transition_confirm)
        {
            pomodoro_show_timeout_popup_locked();
        }
        else
        {
            pomodoro_hide_timeout_popup_locked();
        }
        update_pomodoro_labels_locked();
        _lock_release(&lvgl_api_lock);
    }
}

void pomodoro_screen_start_update_task(void)
{
    if (s_pomodoro_screen_update_task_handle != NULL)
    {
        return;
    }

    BaseType_t task_created = xTaskCreate(pomodoro_screen_update_task,
                                          "pomodoro_screen_update",
                                          4096,
                                          NULL,
                                          5,
                                          &s_pomodoro_screen_update_task_handle);
    if (task_created != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to create pomodoro update task");
        s_pomodoro_screen_update_task_handle = NULL;
        return;
    }

    if (s_pomodoro_countdown_timer_handle == NULL)
    {
        const esp_timer_create_args_t timer_args = {
            .callback = pomodoro_countdown_timer_cb,
            .name = "pomodoro_tick",
        };
        esp_err_t err = esp_timer_create(&timer_args, &s_pomodoro_countdown_timer_handle);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to create countdown timer: %s", esp_err_to_name(err));
            vTaskDelete(s_pomodoro_screen_update_task_handle);
            s_pomodoro_screen_update_task_handle = NULL;
            return;
        }
    }

    esp_err_t err = esp_timer_start_periodic(s_pomodoro_countdown_timer_handle, (uint64_t)POMODORO_SCREEN_UPDATE_MS * 1000ULL);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to start countdown timer: %s", esp_err_to_name(err));
    }

    pomodoro_notify_ui_task();
    ESP_LOGI(TAG, "Pomodoro screen update task started");
}

void pomodoro_screen_stop_update_task(void)
{
    if (s_pomodoro_countdown_timer_handle != NULL)
    {
        (void)esp_timer_stop(s_pomodoro_countdown_timer_handle);
        (void)esp_timer_delete(s_pomodoro_countdown_timer_handle);
        s_pomodoro_countdown_timer_handle = NULL;
    }

    if (s_pomodoro_screen_update_task_handle != NULL)
    {
        vTaskDelete(s_pomodoro_screen_update_task_handle);
        s_pomodoro_screen_update_task_handle = NULL;
    }

    pomodoro_hide_timeout_popup_locked();

    portENTER_CRITICAL(&s_pomodoro_lock);
    s_waiting_transition_confirm = false;
    s_popup_auto_close_ticks = 0;
    portEXIT_CRITICAL(&s_pomodoro_lock);

    ESP_LOGI(TAG, "Pomodoro screen update task stopped");
}

void pomodoro_screen_toggle_pause(void)
{
    portENTER_CRITICAL(&s_pomodoro_lock);
    if (s_waiting_transition_confirm)
    {
        portEXIT_CRITICAL(&s_pomodoro_lock);
        ESP_LOGI(TAG, "Pomodoro waiting for confirmation");
        return;
    }
    s_is_paused = !s_is_paused;
    bool is_paused = s_is_paused;
    portEXIT_CRITICAL(&s_pomodoro_lock);

    pomodoro_notify_ui_task();
    ESP_LOGI(TAG, "Pomodoro %s", is_paused ? "paused" : "resumed");
}

void pomodoro_screen_reset(void)
{
    portENTER_CRITICAL(&s_pomodoro_lock);
    s_is_paused = true;
    s_waiting_transition_confirm = false;
    s_popup_auto_close_ticks = 0;
    if (s_pomodoro_state == POMODORO_STATE_FOCUS)
    {
        s_remaining_seconds = FOCUS_TIME_MINUTES * 60;
    }
    else
    {
        s_remaining_seconds = REST_TIME_MINUTES * 60;
    }
    portEXIT_CRITICAL(&s_pomodoro_lock);

    pomodoro_notify_ui_task();
    ESP_LOGI(TAG, "Pomodoro reset");
}

void pomodoro_screen_skip(void)
{
    portENTER_CRITICAL(&s_pomodoro_lock);
    pomodoro_go_to_next_stage(true);
    s_is_paused = true;
    s_waiting_transition_confirm = false;
    s_popup_auto_close_ticks = 0;
    portEXIT_CRITICAL(&s_pomodoro_lock);

    pomodoro_notify_ui_task();
    ESP_LOGI(TAG, "Pomodoro skipped to next state");
}
