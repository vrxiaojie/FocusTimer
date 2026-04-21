#include <stdio.h>
#include <time.h>
#include <sys/lock.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"

#include "stcc4.h"
#include "lvgl_user.h"
#include "screens.h"

#define MAIN_SCREEN_UPDATE_MS 60000

static const char *TAG = "main_screen_calls";
static TaskHandle_t s_main_screen_update_task_handle = NULL;

static void update_main_screen_labels(const STCC4_value_t *sensor_value)
{
    char hour_text[8];
    char minute_text[8];
    char temp_text[16];
    char humid_text[16];
    char co2_text[16];

    time_t now = time(NULL);
    struct tm time_info = {0};

    if (now > 0)
    {
        localtime_r(&now, &time_info);
    }

    (void)snprintf(hour_text, sizeof(hour_text), "%02d", time_info.tm_hour);
    (void)snprintf(minute_text, sizeof(minute_text), "%02d", time_info.tm_min);
    (void)snprintf(temp_text, sizeof(temp_text), "%.1f℃", sensor_value->temperature);
    (void)snprintf(humid_text, sizeof(humid_text), "%.1f%%", sensor_value->relativeHumidity);
    (void)snprintf(co2_text, sizeof(co2_text), "%dppm", sensor_value->co2Concentration);

    _lock_acquire(&lvgl_api_lock);
    // TODO: 在加入RTC后将此部分改为从RTC获取时间并更新
    if (objects.main_scr_time_hour_label != NULL)
    {
        lv_label_set_text(objects.main_scr_time_hour_label, hour_text);
    }
    if (objects.main_scr_time_minute_label != NULL)
    {
        lv_label_set_text(objects.main_scr_time_minute_label, minute_text);
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
    TickType_t last_wake_time = xTaskGetTickCount();
    STCC4_value_t queue_value = {0};

    while (1)
    {
        if (stcc4_value_queue != NULL && stcc4_task_handle != NULL)
        {
            // 丢弃旧的数据，确保每次更新都是最新的测量值
            while (xQueueReceive(stcc4_value_queue, &queue_value, 0) == pdPASS)
            {
            }

            xTaskNotifyGive(stcc4_task_handle);
            if (xQueueReceive(stcc4_value_queue, &queue_value, pdMS_TO_TICKS(2000)) == pdPASS)
            {
                update_main_screen_labels(&queue_value);
            }
            else
            {
                ESP_LOGW(TAG, "Failed to receive fresh STCC4 data in time");
            }

            xTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(MAIN_SCREEN_UPDATE_MS));
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(200));
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

    xTaskCreate(main_screen_update_task,
                "main_screen_update",
                4096,
                NULL,
                5,
                &s_main_screen_update_task_handle);
    ESP_LOGI(TAG, "Main screen update task started");
}

void main_screen_stop_update_task(void)
{
    if (s_main_screen_update_task_handle != NULL)
    {
        vTaskDelete(s_main_screen_update_task_handle);
        s_main_screen_update_task_handle = NULL;
        ESP_LOGI(TAG, "Main screen update task stopped");
    }
    stcc4_stop_measurement_task();
}
