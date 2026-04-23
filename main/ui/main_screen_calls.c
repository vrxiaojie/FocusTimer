#include <stdio.h>
#include <sys/lock.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "esp_timer.h"

#include "pcf85263a.h"
#include "stcc4.h"
#include "lvgl_user.h"
#include "screens.h"

#define MAIN_SCREEN_UPDATE_MS 60000
#define MAIN_SCREEN_STCC4_WAIT_MS 2000

static const char *TAG = "main_screen_calls";
static TaskHandle_t s_main_screen_update_task_handle = NULL;
static esp_timer_handle_t s_main_screen_timer_handle = NULL;

static const char *const s_weekday_text[] = {
    "星期日",
    "星期一",
    "星期二",
    "星期三",
    "星期四",
    "星期五",
    "星期六",
};

static void main_screen_notify_update_task(void)
{
    if (s_main_screen_update_task_handle != NULL)
    {
        xTaskNotifyGive(s_main_screen_update_task_handle);
    }
}

static void main_screen_timer_cb(void *arg)
{
    (void)arg;
    main_screen_notify_update_task();
}

static void update_main_screen_labels(const STCC4_value_t *sensor_value)
{
    char time_text[12] = "--:--";
    char date_text[24] = "----年--月--日";
    char dayofweek_text[10] = "星期日";
    char temp_text[16];
    char humid_text[16];
    char co2_text[16];

    pcf85263a_datetime_t datetime = {0};
    pcf85263a_handle_t rtc_handle = pcf85263a_get_handle();

    if ((rtc_handle != NULL) && (pcf85263a_get_datetime(rtc_handle, &datetime) == ESP_OK))
    {
        (void)snprintf(time_text, sizeof(time_text), "%02u:%02u", datetime.hour, datetime.minute);
        (void)snprintf(date_text, sizeof(date_text), "%u年%u月%u日", datetime.year, datetime.month, datetime.day);

        if (datetime.weekday < (sizeof(s_weekday_text) / sizeof(s_weekday_text[0])))
        {
            (void)snprintf(dayofweek_text, sizeof(dayofweek_text), "%s", s_weekday_text[datetime.weekday]);
        }
        else
        {
            (void)snprintf(dayofweek_text, sizeof(dayofweek_text), "星期?");
        }
    }

    (void)snprintf(temp_text, sizeof(temp_text), "%.1f℃", sensor_value->temperature);
    (void)snprintf(humid_text, sizeof(humid_text), "%.1f%%", sensor_value->relativeHumidity);
    (void)snprintf(co2_text, sizeof(co2_text), "%dppm", sensor_value->co2Concentration);

    _lock_acquire(&lvgl_api_lock);
    if (objects.main_scr_time_label != NULL)
    {
        lv_label_set_text(objects.main_scr_time_label, time_text);
    }
    if (objects.main_scr_date_value_label != NULL)
    {
        lv_label_set_text(objects.main_scr_date_value_label, date_text);
    }
    if (objects.main_scr_dayofweek_value_label != NULL)
    {
        lv_label_set_text(objects.main_scr_dayofweek_value_label, dayofweek_text);
    }
    if (objects.pomodoro_scr_nowtime_label != NULL)
    {
        lv_label_set_text(objects.pomodoro_scr_nowtime_label, time_text);
    }
    if (objects.main_scr_temp_value_label != NULL)
    {
        lv_label_set_text(objects.main_scr_temp_value_label, temp_text);
    }
    if (objects.main_scr_humid_value_label != NULL)
    {
        lv_label_set_text(objects.main_scr_humid_value_label, humid_text);
    }
    if (objects.main_scr_co2_value_label != NULL)
    {
        lv_label_set_text(objects.main_scr_co2_value_label, co2_text);
    }
    _lock_release(&lvgl_api_lock);
}

static void main_screen_update_task(void *arg)
{
    (void)arg;
    STCC4_value_t queue_value = {0};

    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if (stcc4_value_queue == NULL || stcc4_task_handle == NULL)
        {
            ESP_LOGW(TAG, "STCC4 queue/task not ready");
            continue;
        }

        // 丢弃旧的数据，确保每次更新都是最新的测量值
        while (xQueueReceive(stcc4_value_queue, &queue_value, 0) == pdPASS)
        {
        }

        xTaskNotifyGive(stcc4_task_handle);
        if (xQueueReceive(stcc4_value_queue, &queue_value, pdMS_TO_TICKS(MAIN_SCREEN_STCC4_WAIT_MS)) == pdPASS)
        {
            update_main_screen_labels(&queue_value);
        }
        else
        {
            ESP_LOGW(TAG, "Failed to receive fresh STCC4 data in time");
        }
    }
}

void main_screen_start_update_task(void)
{
    if (stcc4_task_handle == NULL)
    {
        stcc4_start_measurement_task();
    }

    if (s_main_screen_update_task_handle != NULL)
    {
        return;
    }

    BaseType_t task_created = xTaskCreate(main_screen_update_task,
                                          "main_screen_update",
                                          4096,
                                          NULL,
                                          5,
                                          &s_main_screen_update_task_handle);
    if (task_created != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to create main screen update task");
        s_main_screen_update_task_handle = NULL;
        return;
    }

    if (s_main_screen_timer_handle == NULL)
    {
        const esp_timer_create_args_t timer_args = {
            .callback = main_screen_timer_cb,
            .name = "main_scr_tick",
        };
        esp_err_t err = esp_timer_create(&timer_args, &s_main_screen_timer_handle);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to create main screen timer: %s", esp_err_to_name(err));
            vTaskDelete(s_main_screen_update_task_handle);
            s_main_screen_update_task_handle = NULL;
            return;
        }
    }

    esp_err_t err = esp_timer_start_periodic(s_main_screen_timer_handle, (uint64_t)MAIN_SCREEN_UPDATE_MS * 1000ULL);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to start main screen timer: %s", esp_err_to_name(err));
    }

    // 启动后立即刷新一次，避免等待第一个周期
    main_screen_notify_update_task();
    ESP_LOGI(TAG, "Main screen update task started");
}

void main_screen_stop_update_task(void)
{
    if (s_main_screen_timer_handle != NULL)
    {
        (void)esp_timer_stop(s_main_screen_timer_handle);
        (void)esp_timer_delete(s_main_screen_timer_handle);
        s_main_screen_timer_handle = NULL;
    }

    if (s_main_screen_update_task_handle != NULL)
    {
        vTaskDelete(s_main_screen_update_task_handle);
        s_main_screen_update_task_handle = NULL;
        ESP_LOGI(TAG, "Main screen update task stopped");
    }
    stcc4_stop_measurement_task();
}
