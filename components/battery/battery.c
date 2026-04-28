#include <stdio.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_check.h"

#include "adc_battery_estimation.h"
#include "aw32001.h"
#include "battery.h"

#define TAG "battery"

#define BATTERY_TASK_STACK_SIZE (3072)
#define BATTERY_TASK_PRIORITY (3)
#define BATTERY_DETECT_INTERVAL_MS (60000)

/*
 * 注意：下面 ADC / 分压参数需按硬件原理图校准。
 */
#define BATTERY_ADC_UNIT (ADC_UNIT_1)
#define BATTERY_ADC_BITWIDTH (ADC_BITWIDTH_DEFAULT)
#define BATTERY_ADC_ATTEN (ADC_ATTEN_DB_2_5)
#define BATTERY_ADC_CHANNEL (ADC_CHANNEL_1)
#define BATTERY_DIVIDER_UPPER_OHM (1500.0f)
#define BATTERY_DIVIDER_LOWER_OHM (499.0f)

static adc_battery_estimation_handle_t s_battery_handle = NULL;
static TaskHandle_t s_battery_task_handle = NULL;

static float s_capacity = 0.0f;
static bool s_is_charging = false;

static bool battery_charging_detect_cb(void *user_data)
{
    (void)user_data;

    aw32001_sys_status_t sys_status = {0};
    if (aw32001_read_sys_status(&sys_status) != ESP_OK)
    {
        ESP_LOGW(TAG, "read aw32001 sys status failed, keep last charging state=%d", s_is_charging);
        return s_is_charging;
    }

    /*
     * 依据 AW32001 REG08H CHG_STAT 位判断：
     * 00: Not Charging
     * 01: Pre Charge
     * 02: Fast Charge
     * 03: Charge Done
     *
     * 其中 01/02/03 视为已插入充电器（处于充电路径有效）。
     */
    return (sys_status.chg_stat != AW32001_CHG_STAT_NOT_CHARGING);
}

static void battery_monitor_task(void *arg)
{
    (void)arg;

    while (1)
    {
        float capacity = 0.0f;
        bool is_charging = false;

        esp_err_t err_cap = adc_battery_estimation_get_capacity(s_battery_handle, &capacity);
        esp_err_t err_chg = adc_battery_estimation_get_charging_state(s_battery_handle, &is_charging);

        if (err_cap == ESP_OK)
        {
            s_capacity = capacity;
        }
        else
        {
            ESP_LOGW(TAG, "get battery capacity failed: %s", esp_err_to_name(err_cap));
        }

        if (err_chg == ESP_OK)
        {
            s_is_charging = is_charging;
        }
        else
        {
            ESP_LOGW(TAG, "get charging state failed: %s", esp_err_to_name(err_chg));
        }

        ESP_LOGI(TAG, "battery capacity=%.1f%%, charging=%s", s_capacity, s_is_charging ? "true" : "false");

        vTaskDelay(pdMS_TO_TICKS(BATTERY_DETECT_INTERVAL_MS));
    }
}

esp_err_t battery_init(void)
{
    aw32001_disable_watchdog();
    aw32001_enable_charge();
    aw32001_set_chg_current(512);

    if (s_battery_handle != NULL)
    {
        ESP_LOGW(TAG, "battery already initialized");
        return ESP_OK;
    }

    adc_battery_estimation_t config = {
        .internal = {
            .adc_unit = BATTERY_ADC_UNIT,
            .adc_bitwidth = BATTERY_ADC_BITWIDTH,
            .adc_atten = BATTERY_ADC_ATTEN,
        },
        .adc_channel = BATTERY_ADC_CHANNEL,
        .upper_resistor = BATTERY_DIVIDER_UPPER_OHM,
        .lower_resistor = BATTERY_DIVIDER_LOWER_OHM,
        .charging_detect_cb = battery_charging_detect_cb,
        .charging_detect_user_data = NULL,
    };

    s_battery_handle = adc_battery_estimation_create(&config);
    ESP_RETURN_ON_FALSE(s_battery_handle != NULL, ESP_FAIL, TAG, "adc_battery_estimation_create failed");

    BaseType_t ok = xTaskCreate(
        battery_monitor_task,
        "battery_task",
        BATTERY_TASK_STACK_SIZE,
        NULL,
        BATTERY_TASK_PRIORITY,
        &s_battery_task_handle);

    if (ok != pdPASS)
    {
        ESP_LOGE(TAG, "create battery task failed");
        adc_battery_estimation_destroy(s_battery_handle);
        s_battery_handle = NULL;
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "battery init done");
    return ESP_OK;
}

float battery_get_capacity(void)
{
    return s_capacity;
}

bool battery_is_charging(void)
{
    return s_is_charging;
}
