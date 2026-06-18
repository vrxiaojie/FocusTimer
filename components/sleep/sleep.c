#include "sleep.h"

#include <stdio.h>
#include <sys/lock.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_sleep.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "lvgl.h"
#include "ui.h"
#include "screens.h"

#include "spi_shared_lock.h"
#include "lvgl_user.h"
#include "sys_init.h"
#include "aw96103.h"
#include "stcc4.h"
#include "pcf85263a.h"
#include "imu.h"
#include "aw32001.h"
#include "battery.h"
#include "max98357.h"
#include "message_screen_calls.h"
#include "main_screen_calls.h"
#include "power_setting_screen_calls.h"
#include "nvs_storage.h"
#include "power_management.h"
#include "st7305_2p9.h"
#include "ble.h"
#include "driver/gpio.h"

#define TAG "sleep"

/* ---- 休眠时段 NVS 键名（与 power_setting_screen_calls.c 共用） ---- */
#define POWER_MGMT_NVS_NAMESPACE "power_mgr"
#define NVS_KEY_SLEEP_START_HOUR   "pm_sleep_sh"
#define NVS_KEY_SLEEP_START_MINUTE "pm_sleep_sm"
#define NVS_KEY_SLEEP_END_HOUR     "pm_sleep_eh"
#define NVS_KEY_SLEEP_END_MINUTE   "pm_sleep_em"
#define DEFAULT_SLEEP_START_HOUR   22
#define DEFAULT_SLEEP_START_MINUTE 0
#define DEFAULT_SLEEP_END_HOUR     6
#define DEFAULT_SLEEP_END_MINUTE   0
#define SECONDS_PER_DAY            (24U * 60U * 60U)

typedef struct
{
    uint8_t start_hour;
    uint8_t start_minute;
    uint8_t end_hour;
    uint8_t end_minute;
} power_sleep_period_t;

static bool s_skip_next_pre_deepsleep_record_save = false;

/* ==================== NVS 辅助 ==================== */

static uint8_t nvs_read_u8_default(const char *key, uint8_t default_val)
{
    nvs_handle_t handle;
    uint8_t value = default_val;

    if (nvs_open(POWER_MGMT_NVS_NAMESPACE, NVS_READONLY, &handle) == ESP_OK)
    {
        if (nvs_get_u8(handle, key, &value) != ESP_OK)
        {
            value = default_val;
        }
        nvs_close(handle);
    }

    return value;
}

static void load_power_sleep_period(power_sleep_period_t *period)
{
    if (period == NULL)
    {
        return;
    }

    period->start_hour   = nvs_read_u8_default(NVS_KEY_SLEEP_START_HOUR,   DEFAULT_SLEEP_START_HOUR);
    period->start_minute = nvs_read_u8_default(NVS_KEY_SLEEP_START_MINUTE, DEFAULT_SLEEP_START_MINUTE);
    period->end_hour     = nvs_read_u8_default(NVS_KEY_SLEEP_END_HOUR,     DEFAULT_SLEEP_END_HOUR);
    period->end_minute   = nvs_read_u8_default(NVS_KEY_SLEEP_END_MINUTE,   DEFAULT_SLEEP_END_MINUTE);

    if (period->start_hour > 23)   period->start_hour   = 0;
    if (period->start_minute > 59) period->start_minute = 0;
    if (period->end_hour > 23)     period->end_hour     = 0;
    if (period->end_minute > 59)   period->end_minute   = 0;
}

/* ==================== 休眠时段判断 ==================== */

static bool get_sleep_period_remaining_ms(const power_sleep_period_t *period,
                                          const pcf85263a_datetime_t *datetime,
                                          uint32_t *out_remaining_ms)
{
    if (period == NULL || datetime == NULL || out_remaining_ms == NULL)
    {
        return false;
    }

    uint32_t start_seconds = ((uint32_t)period->start_hour * 60U + (uint32_t)period->start_minute) * 60U;
    uint32_t end_seconds   = ((uint32_t)period->end_hour * 60U + (uint32_t)period->end_minute) * 60U;
    uint32_t now_seconds   = ((uint32_t)datetime->hour * 60U + (uint32_t)datetime->minute) * 60U
                           + (uint32_t)datetime->second;
    uint32_t remaining_seconds = 0;

    if (start_seconds == end_seconds)
    {
        return false; /* 开始==结束 视为禁用 */
    }

    if (start_seconds < end_seconds)
    {
        /* 同日时段：如 06:00~22:00 */
        if (now_seconds < start_seconds || now_seconds >= end_seconds)
        {
            return false;
        }
        remaining_seconds = end_seconds - now_seconds;
    }
    else
    {
        /* 跨日时段：如 22:00~06:00 */
        if (now_seconds >= start_seconds)
        {
            remaining_seconds = (SECONDS_PER_DAY - now_seconds) + end_seconds;
        }
        else if (now_seconds < end_seconds)
        {
            remaining_seconds = end_seconds - now_seconds;
        }
        else
        {
            return false;
        }
    }

    if (remaining_seconds == 0)
    {
        return false;
    }

    *out_remaining_ms = remaining_seconds * 1000U;
    return true;
}

/**
 * @brief 判断当前是否处于配置的休眠时段，如果是则保存记录、关屏、直接 deep sleep
 * @return true 已进入 deep sleep（不会返回），false 不在休眠时段
 */
static bool maybe_enter_configured_sleep_period(void)
{
    power_sleep_period_t period = {0};
    pcf85263a_datetime_t datetime = {0};
    pcf85263a_handle_t rtc_handle = pcf85263a_get_handle();

    if (rtc_handle == NULL)
    {
        ESP_LOGW(TAG, "rtc handle unavailable when checking sleep period");
        return false;
    }

    load_power_sleep_period(&period);

    esp_err_t err = pcf85263a_get_datetime(rtc_handle, &datetime);
    if (err != ESP_OK)
    {
        ESP_LOGW(TAG, "read rtc datetime for sleep period failed: %s", esp_err_to_name(err));
        return false;
    }

    uint32_t sleep_ms = 0;
    if (!get_sleep_period_remaining_ms(&period, &datetime, &sleep_ms))
    {
        return false;
    }

    /* 在休眠时段内，先落盘历史记录 */
    err = nvs_storage_save_daily_record();
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "save daily record before configured sleep failed: %s", esp_err_to_name(err));
    }

    ESP_LOGI(TAG,
             "in configured sleep period %02u:%02u-%02u:%02u, now=%02u:%02u:%02u, RTC alarm at %02u:%02u",
             period.start_hour, period.start_minute,
             period.end_hour, period.end_minute,
             datetime.hour, datetime.minute, datetime.second,
             period.end_hour, period.end_minute);

    /* 关屏幕：最小初始化 SPI + LCD */
    spi_shared_lock_init();
    ESP_ERROR_CHECK_WITHOUT_ABORT(spi_bus_init());
    ESP_ERROR_CHECK_WITHOUT_ABORT(lcd_screen_init());
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_lcd_panel_disp_on_off(panel_handle, false));

    /* 因为进入休眠时段后不会过0点，这里需要做午夜同步 */
    sleep_sync_daily_record_on_midnight_wakeup();

    power_management_enter_deepsleep_until_rtc_time(period.end_hour, period.end_minute);
    return true;
}

/* ==================== 回调 ==================== */

static void shipping_mode_cb(void *user_data)
{
    (void)user_data;

    esp_err_t err = nvs_storage_save_daily_record();
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "save daily record before shipping mode failed: %s", esp_err_to_name(err));
    }

    _lock_acquire(&lvgl_api_lock);
    message_screen_show_with_text("", "关机中...\n如果正在充电,拔下电源后才会关机", "OK");
    _lock_release(&lvgl_api_lock);
}

static void pre_deepsleep_cb(void *user_data)
{
    (void)user_data;

    (void)ble_set_advertising_enabled(false);

    bool save_record = !s_skip_next_pre_deepsleep_record_save;
    s_skip_next_pre_deepsleep_record_save = false;

    esp_err_t err = ESP_OK;
    if (save_record)
    {
        err = nvs_storage_save_daily_record();
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "save daily record before deep sleep failed: %s", esp_err_to_name(err));
        }
    }

    if (aw96103_is_initialized())
    {
        err = aw96103_enter_doze_mode();
        if (err != ESP_OK)
        {
            ESP_LOGW(TAG, "set aw96103 doze mode before deep sleep failed: %s", esp_err_to_name(err));
        }
    }

    err = stcc4_prepare_for_deepsleep();
    if (err != ESP_OK)
    {
        ESP_LOGW(TAG, "set stcc4 sleep mode before deep sleep failed: %s", esp_err_to_name(err));
    }

    err = max98357_prepare_for_deepsleep();
    if (err != ESP_OK)
    {
        ESP_LOGW(TAG, "set audio amplifier shutdown before deep sleep failed: %s", esp_err_to_name(err));
    }

    if (panel_handle != NULL)
    {
        esp_lcd_panel_st7305_set_power_mode(panel_handle, ST7305_PWR_MODE_LPM);
    }

    (void)imu_prepare_for_deepsleep();

#if !SOC_GPIO_SUPPORT_HOLD_SINGLE_IO_IN_DSLP
    gpio_deep_sleep_hold_en();
#endif
}

static void ble_datetime_updated_cb(void *user_data)
{
    (void)user_data;
    update_main_screen_date_labels(true);
}

static void ble_power_settings_updated_cb(void *user_data)
{
    (void)user_data;

    _lock_acquire(&lvgl_api_lock);
    power_setting_screen_reload_from_storage();
    _lock_release(&lvgl_api_lock);
}

/* ==================== 最小显示栈 ==================== */

static void init_minimal_display_stack(void)
{
    spi_shared_lock_init();
    ESP_ERROR_CHECK_WITHOUT_ABORT(spi_bus_init());
    ESP_ERROR_CHECK_WITHOUT_ABORT(lcd_screen_init());
    lvgl_user_init(panel_handle, io_handle);

    _lock_acquire(&lvgl_api_lock);
    create_screens();
    lv_scr_load(objects.main);
    if (lvgl_display != NULL)
    {
        lv_refr_now(lvgl_display);
    }
    _lock_release(&lvgl_api_lock);

    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
}

/* ==================== 公开接口 ==================== */

esp_err_t sleep_init_nvs_flash(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK_WITHOUT_ABORT(nvs_flash_erase());
        err = nvs_flash_init();
    }
    return err;
}

void sleep_sync_daily_record_on_midnight_wakeup(void)
{
    bool is_midnight = false;
    esp_err_t err = nvs_storage_is_midnight(&is_midnight);
    if (err != ESP_OK)
    {
        ESP_LOGW(TAG, "midnight check failed: %s", esp_err_to_name(err));
        return;
    }

    if (!is_midnight)
    {
        return;
    }

    err = nvs_storage_sync_current_day();
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "midnight rollover sync failed: %s", esp_err_to_name(err));
        return;
    }

    ESP_LOGI(TAG, "midnight wakeup detected, synced previous day record");
}

void sleep_handle_rtc_wakeup(void)
{
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_init());
    ESP_ERROR_CHECK_WITHOUT_ABORT(pcf85263a_init(I2C_NUM_0));
    pcf85263a_handle_t rtc_handle = pcf85263a_get_handle();
    if (rtc_handle != NULL)
    {
        ESP_ERROR_CHECK_WITHOUT_ABORT(pcf85263a_clear_flags(rtc_handle, PCF85263A_FLAG_PIF | PCF85263A_FLAG_A1F));
    }
    ESP_ERROR_CHECK_WITHOUT_ABORT(nvs_storage_init());
    ESP_ERROR_CHECK_WITHOUT_ABORT(stcc4_i2c_init(I2C_NUM_0));
    ESP_ERROR_CHECK_WITHOUT_ABORT(aw32001_init(I2C_NUM_0));
    ESP_ERROR_CHECK_WITHOUT_ABORT(battery_init());
    ESP_ERROR_CHECK_WITHOUT_ABORT(battery_refresh_once());
    sleep_register_pre_deepsleep_cb();

    /* 如果当前处于配置的休眠时段，关屏并继续 deep sleep 到时段结束 */
    if (maybe_enter_configured_sleep_period())
    {
        return;
    }

    main_screen_set_auto_wakeup_refreshing(true);
    init_minimal_display_stack();

    esp_lcd_panel_st7305_set_power_mode(panel_handle, ST7305_PWR_MODE_HPM);
    (void)main_screen_refresh_once(1200);
    main_screen_set_auto_wakeup_refreshing(false);

    _lock_acquire(&lvgl_api_lock);
    if (lvgl_display != NULL)
    {
        lv_refr_now(lvgl_display);
    }
    _lock_release(&lvgl_api_lock);
    vTaskDelay(pdMS_TO_TICKS(120));

    esp_lcd_panel_st7305_set_power_mode(panel_handle, ST7305_PWR_MODE_LPM);
    s_skip_next_pre_deepsleep_record_save = true;
    power_management_enter_deepsleep(60 * 1000);
}

void sleep_register_pre_deepsleep_cb(void)
{
    (void)power_management_register_pre_deepsleep_cb(pre_deepsleep_cb, NULL);
}

void sleep_register_shipping_mode_cb(void)
{
    aw32001_register_shipping_mode_cb(shipping_mode_cb, NULL);
}

void sleep_register_ble_datetime_cb(void)
{
    ble_register_datetime_updated_cb(ble_datetime_updated_cb, NULL);
}

void sleep_register_ble_power_settings_cb(void)
{
    ble_register_power_settings_updated_cb(ble_power_settings_updated_cb, NULL);
}
