#include <stdio.h>
#include <sys/lock.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_timer.h"
#include "esp_log.h"

#include "lvgl_user.h"
#include "screens.h"
#include "message_screen_calls.h"
#include "pomodoro_screen_calls.h"
#include "imu.h"

#define POMODORO_SCREEN_UPDATE_MS 1000
#define POMODORO_FLIP_CHECK_MS    1000

static const char *TAG = "pomodoro_screen";
static TaskHandle_t s_pomodoro_screen_update_task_handle = NULL;
static esp_timer_handle_t s_pomodoro_countdown_timer_handle = NULL;
static esp_timer_handle_t s_pomodoro_flip_check_timer_handle = NULL;
static bool s_update_task_exit_requested = false;
static bool s_countdown_timer_running = false;

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
static bool s_timeout_message_pending = false;

// 默认时间设置
static const uint8_t FOCUS_TIME_MINUTES = 25;
static const uint8_t REST_TIME_MINUTES = 5;

static portMUX_TYPE s_pomodoro_lock = portMUX_INITIALIZER_UNLOCKED;

static void pomodoro_play_timeout_audio_todo(void)
{
    // TODO: 接入音频播放 API，在倒计时结束时播放提示音。
}

static void pomodoro_update_countdown_timer_locked(void)
{
    if (s_pomodoro_countdown_timer_handle == NULL)
    {
        return;
    }

    bool should_run = (!s_is_paused) && (!s_waiting_transition_confirm);

    if (should_run && !s_countdown_timer_running)
    {
        esp_err_t err = esp_timer_start_periodic(s_pomodoro_countdown_timer_handle,
                                                 (uint64_t)POMODORO_SCREEN_UPDATE_MS * 1000ULL);
        if (err == ESP_OK)
        {
            s_countdown_timer_running = true;
        }
        else if (err != ESP_ERR_INVALID_STATE)
        {
            ESP_LOGE(TAG, "Failed to start countdown timer: %s", esp_err_to_name(err));
        }
    }
    else if (!should_run && s_countdown_timer_running)
    {
        esp_err_t err = esp_timer_stop(s_pomodoro_countdown_timer_handle);
        if (err == ESP_OK)
        {
            s_countdown_timer_running = false;
        }
        else if (err != ESP_ERR_INVALID_STATE)
        {
            ESP_LOGE(TAG, "Failed to stop countdown timer: %s", esp_err_to_name(err));
        }
    }
}

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

static void pomodoro_timeout_message_confirm_cb(void *user_data)
{
    (void)user_data;

    portENTER_CRITICAL(&s_pomodoro_lock);
    pomodoro_go_to_next_stage(true);
    s_waiting_transition_confirm = false;
    s_timeout_message_pending = false;
    s_is_paused = true;
    pomodoro_update_countdown_timer_locked();
    portEXIT_CRITICAL(&s_pomodoro_lock);

    pomodoro_notify_ui_task();
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

    portENTER_CRITICAL(&s_pomodoro_lock);
    if (!s_is_paused && !s_waiting_transition_confirm)
    {
        if (s_remaining_seconds > 0)
        {
            s_remaining_seconds--;
        }

        if (s_remaining_seconds == 0)
        {
            pomodoro_play_timeout_audio_todo();
            s_waiting_transition_confirm = true;
            s_is_paused = true;
            s_timeout_message_pending = true;
            s_countdown_timer_running = false;
        }
    }
    portEXIT_CRITICAL(&s_pomodoro_lock);
    pomodoro_notify_ui_task();
}

static void pomodoro_flip_check_timer_cb(void *arg)
{
    (void)arg;
    static lv_disp_rotation_t s_last_imu_rotation = LV_DISP_ROTATION_0;
    static bool s_flip_rotation_initialized = false;

    lv_disp_rotation_t imu_rotation = lvgl_user_get_rotation();
    if (!s_flip_rotation_initialized)
    {
        s_last_imu_rotation = imu_rotation;
        s_flip_rotation_initialized = true;
        return;
    }

    if (imu_rotation != s_last_imu_rotation)
    {
        s_last_imu_rotation = imu_rotation;
        pomodoro_screen_toggle_pause();
    }
}

static void pomodoro_screen_update_task(void *arg)
{
    (void)arg;
    pomodoro_notify_ui_task();

    while (1)
    {
        if (s_update_task_exit_requested)
        {
            break;
        }

        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (s_update_task_exit_requested)
        {
            break;
        }

        bool show_timeout_message = false;
        pomodoro_state_t state_before_transition = POMODORO_STATE_FOCUS;
        char message_content[48];

        portENTER_CRITICAL(&s_pomodoro_lock);
        if (s_timeout_message_pending)
        {
            show_timeout_message = true;
            state_before_transition = s_pomodoro_state;
            s_timeout_message_pending = false;
        }
        portEXIT_CRITICAL(&s_pomodoro_lock);

        if (show_timeout_message)
        {
            (void)snprintf(message_content,
                           sizeof(message_content),
                           "点击OK进入%s时间",
                           state_before_transition == POMODORO_STATE_FOCUS ? "休息" : "专注");
        }

        _lock_acquire(&lvgl_api_lock);
        update_pomodoro_labels_locked();
        if (show_timeout_message)
        {
            message_screen_set_confirm_callback(pomodoro_timeout_message_confirm_cb, NULL);
            message_screen_show_with_text("时间到", message_content, "OK");
        }
        _lock_release(&lvgl_api_lock);
    }

    vTaskDelete(NULL);
}

void pomodoro_screen_start_update_task(void)
{
    if (s_pomodoro_screen_update_task_handle != NULL)
    {
        return;
    }

    s_update_task_exit_requested = false;

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
        const esp_timer_create_args_t countdown_timer_args = {
            .callback = pomodoro_countdown_timer_cb,
            .name = "pomodoro_tick",
        };
        esp_err_t err = esp_timer_create(&countdown_timer_args, &s_pomodoro_countdown_timer_handle);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to create countdown timer: %s", esp_err_to_name(err));
            vTaskDelete(s_pomodoro_screen_update_task_handle);
            s_pomodoro_screen_update_task_handle = NULL;
            return;
        }
    }

    if (s_pomodoro_flip_check_timer_handle == NULL)
    {
        const esp_timer_create_args_t flip_timer_args = {
            .callback = pomodoro_flip_check_timer_cb,
            .name = "pomodoro_flip",
        };
        esp_err_t err = esp_timer_create(&flip_timer_args, &s_pomodoro_flip_check_timer_handle);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to create flip timer: %s", esp_err_to_name(err));
            (void)esp_timer_delete(s_pomodoro_countdown_timer_handle);
            s_pomodoro_countdown_timer_handle = NULL;
            vTaskDelete(s_pomodoro_screen_update_task_handle);
            s_pomodoro_screen_update_task_handle = NULL;
            return;
        }
    }

    portENTER_CRITICAL(&s_pomodoro_lock);
    s_countdown_timer_running = false;
    s_pomodoro_countdown_timer_handle = s_pomodoro_countdown_timer_handle;
    pomodoro_update_countdown_timer_locked();
    portEXIT_CRITICAL(&s_pomodoro_lock);

    esp_err_t err = esp_timer_start_periodic(s_pomodoro_flip_check_timer_handle, (uint64_t)POMODORO_FLIP_CHECK_MS * 1000ULL);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to start flip timer: %s", esp_err_to_name(err));
    }

    pomodoro_notify_ui_task();
    // ESP_LOGI(TAG, "Pomodoro screen update task started");
}

void pomodoro_screen_stop_update_task(void)
{
    s_update_task_exit_requested = true;
    s_countdown_timer_running = false;

    if (s_pomodoro_countdown_timer_handle != NULL)
    {
        (void)esp_timer_stop(s_pomodoro_countdown_timer_handle);
        (void)esp_timer_delete(s_pomodoro_countdown_timer_handle);
        s_pomodoro_countdown_timer_handle = NULL;
    }

    if (s_pomodoro_flip_check_timer_handle != NULL)
    {
        (void)esp_timer_stop(s_pomodoro_flip_check_timer_handle);
        (void)esp_timer_delete(s_pomodoro_flip_check_timer_handle);
        s_pomodoro_flip_check_timer_handle = NULL;
    }

    if (s_pomodoro_screen_update_task_handle != NULL)
    {
        TaskHandle_t task_handle = s_pomodoro_screen_update_task_handle;
        s_pomodoro_screen_update_task_handle = NULL;
        xTaskNotifyGive(task_handle);
    }

    // ESP_LOGI(TAG, "Pomodoro screen update task stopped");
}

void pomodoro_screen_toggle_pause(void)
{
    portENTER_CRITICAL(&s_pomodoro_lock);
    if (s_waiting_transition_confirm)
    {
        portEXIT_CRITICAL(&s_pomodoro_lock);
        // ESP_LOGI(TAG, "Pomodoro waiting for confirmation");
        return;
    }
    s_is_paused = !s_is_paused;
    pomodoro_update_countdown_timer_locked();
    portEXIT_CRITICAL(&s_pomodoro_lock);

    pomodoro_notify_ui_task();
    // ESP_LOGI(TAG, "Pomodoro %s", s_is_paused ? "paused" : "resumed");
}

void pomodoro_screen_reset(void)
{
    portENTER_CRITICAL(&s_pomodoro_lock);
    s_is_paused = true;
    s_waiting_transition_confirm = false;
    s_timeout_message_pending = false;
    if (s_pomodoro_state == POMODORO_STATE_FOCUS)
    {
        s_remaining_seconds = FOCUS_TIME_MINUTES * 60;
    }
    else
    {
        s_remaining_seconds = REST_TIME_MINUTES * 60;
    }
    pomodoro_update_countdown_timer_locked();
    portEXIT_CRITICAL(&s_pomodoro_lock);

    pomodoro_notify_ui_task();
    // ESP_LOGI(TAG, "Pomodoro reset");
}

void pomodoro_screen_skip(void)
{
    portENTER_CRITICAL(&s_pomodoro_lock);
    pomodoro_go_to_next_stage(true);
    s_is_paused = true;
    s_waiting_transition_confirm = false;
    s_timeout_message_pending = false;
    pomodoro_update_countdown_timer_locked();
    portEXIT_CRITICAL(&s_pomodoro_lock);

    pomodoro_notify_ui_task();
    // ESP_LOGI(TAG, "Pomodoro skipped to next state");
}
