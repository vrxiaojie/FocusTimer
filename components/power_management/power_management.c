#include "power_management.h"

#include <stdbool.h>
#include <stdint.h>

#include "esp_log.h"
#include "esp_check.h"
#include "esp_pm.h"
#include "esp_sleep.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "st7305_2p9.h"
#include "pinmap.h"
#include "pcf85263a.h"

#define TAG "power_mgmt"

/* ---- NVS 键名 ---- */
#define NVS_NAMESPACE "power_mgr"
#define NVS_KEY_AUTO_LPM "pm_auto_lpm"
#define NVS_KEY_AUTO_SLEEP "pm_auto_sleep"
#define NVS_KEY_CHG_THR "pm_chg_thr"

/* ---- 默认值 ---- */
#define DEFAULT_AUTO_LPM false
#define DEFAULT_AUTO_SLEEP false
#define DEFAULT_CHG_THRESHOLD 90

/* ---- 空闲超时 ---- */
#define SCREEN_LPM_TIMEOUT_SEC (2)
#define DEEPSLEEP_IDLE_TIMEOUT_SEC (5 * 60)
#define DEFAULT_RTC_WAKEUP_MS (60 * 1000U)

/* ---- 频率配置 ---- */
#define PM_MAX_FREQ_MHZ 96
#define PM_MIN_FREQ_MHZ 48

/* ---- 运行时状态 ---- */
static bool s_auto_lightsleep = DEFAULT_AUTO_LPM;
static bool s_auto_sleep = DEFAULT_AUTO_SLEEP;

static power_management_hook_cb_t s_pre_deepsleep_cb = NULL;
static void *s_pre_deepsleep_user_data = NULL;
static uint8_t s_charge_threshold = DEFAULT_CHG_THRESHOLD;
static esp_lcd_panel_handle_t s_panel_handle = NULL;

static esp_timer_handle_t s_idle_timer = NULL;
static int32_t s_idle_seconds = 0;
static int32_t s_screen_idle_seconds = 0;
static bool s_screen_in_lpm = false;
static bool s_deepsleep_idle_detect_enabled = false;

static void set_screen_power_mode(st7305_power_mode_t power_mode)
{
    if (s_panel_handle == NULL)
        return;

    bool target_lpm = (power_mode == ST7305_PWR_MODE_LPM);
    if (s_screen_in_lpm == target_lpm)
        return;

    esp_err_t err = esp_lcd_panel_st7305_set_power_mode(s_panel_handle, power_mode);
    if (err != ESP_OK)
    {
        ESP_LOGW(TAG, "set screen power mode %s failed: %s",
                 target_lpm ? "LPM" : "HPM", esp_err_to_name(err));
        return;
    }

    s_screen_in_lpm = target_lpm;
}

/* ==================== NVS 辅助 ==================== */

static esp_err_t nvs_read_bool(const char *key, bool default_val, bool *out)
{
    nvs_handle_t h;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &h);
    if (err != ESP_OK)
    {
        *out = default_val;
        return ESP_OK; /* 首次运行无记录，使用默认值 */
    }
    uint8_t val = default_val ? 1 : 0;
    err = nvs_get_u8(h, key, &val);
    nvs_close(h);
    *out = (val != 0);
    return (err == ESP_OK) ? ESP_OK : ESP_OK; /* 键不存在也返回 OK + 默认值 */
}

static esp_err_t nvs_write_bool(const char *key, bool val)
{
    nvs_handle_t h;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &h);
    if (err != ESP_OK)
        return err;
    err = nvs_set_u8(h, key, val ? 1 : 0);
    nvs_commit(h);
    nvs_close(h);
    return err;
}

static esp_err_t nvs_read_u8(const char *key, uint8_t default_val, uint8_t *out)
{
    nvs_handle_t h;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &h);
    if (err != ESP_OK)
    {
        *out = default_val;
        return ESP_OK;
    }
    err = nvs_get_u8(h, key, out);
    nvs_close(h);
    if (err != ESP_OK)
        *out = default_val;
    return ESP_OK;
}

static esp_err_t nvs_write_u8(const char *key, uint8_t val)
{
    nvs_handle_t h;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &h);
    if (err != ESP_OK)
        return err;
    err = nvs_set_u8(h, key, val);
    nvs_commit(h);
    nvs_close(h);
    return err;
}

/* ==================== 低功耗模式 ==================== */

static esp_err_t apply_auto_lightsleep(bool enable)
{
    if (enable)
    {
        esp_pm_config_t pm_config = {
            .max_freq_mhz = PM_MAX_FREQ_MHZ,
            .min_freq_mhz = PM_MIN_FREQ_MHZ,
            .light_sleep_enable = true,
        };
        esp_err_t err = esp_pm_configure(&pm_config);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "esp_pm_configure enable failed: %s", esp_err_to_name(err));
        }
        else
        {
            ESP_LOGI(TAG, "auto light sleep enabled (%d/%d MHz)", PM_MAX_FREQ_MHZ, PM_MIN_FREQ_MHZ);
        }
        return err;
    }
    else
    {
        /* 关闭时恢复默认：不启用 light sleep */
        esp_pm_config_t pm_config = {
            .max_freq_mhz = PM_MAX_FREQ_MHZ,
            .min_freq_mhz = PM_MAX_FREQ_MHZ,
            .light_sleep_enable = false,
        };
        esp_err_t err = esp_pm_configure(&pm_config);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "esp_pm_configure disable failed: %s", esp_err_to_name(err));
        }
        else
        {
            ESP_LOGI(TAG, "auto light sleep disabled");
        }
        return err;
    }
}

esp_err_t power_management_set_auto_lightsleep(bool enable)
{
    s_auto_lightsleep = enable;
    nvs_write_bool(NVS_KEY_AUTO_LPM, enable);
    return apply_auto_lightsleep(enable);
}

bool power_management_get_auto_lightsleep(void)
{
    return s_auto_lightsleep;
}

void power_management_register_panel(esp_lcd_panel_handle_t panel)
{
    s_panel_handle = panel;
    s_screen_in_lpm = false;
}

void power_management_notify_user_activity(void)
{
    set_screen_power_mode(ST7305_PWR_MODE_HPM);

    power_management_reset_idle_timer();
}

/* ==================== 自动休眠 ==================== */

esp_err_t power_management_set_auto_sleep(bool enable)
{
    s_auto_sleep = enable;
    nvs_write_bool(NVS_KEY_AUTO_SLEEP, enable);
    if (!enable)
    {
        s_idle_seconds = 0;
    }
    ESP_LOGI(TAG, "auto sleep %s", enable ? "enabled" : "disabled");
    return ESP_OK;
}

bool power_management_get_auto_sleep(void)
{
    return s_auto_sleep;
}

/* ==================== 充电阈值 ==================== */

esp_err_t power_management_set_charge_threshold(uint8_t threshold)
{
    if (threshold > 100)
        threshold = 100;
    s_charge_threshold = threshold;
    nvs_write_u8(NVS_KEY_CHG_THR, threshold);
    ESP_LOGI(TAG, "charge threshold set to %d%%", threshold);
    return ESP_OK;
}

uint8_t power_management_get_charge_threshold(void)
{
    return s_charge_threshold;
}

/* ==================== 空闲检测定时器 ==================== */

static void idle_timer_cb(void *arg)
{
    (void)arg;

    s_screen_idle_seconds++;
    if (s_screen_idle_seconds >= SCREEN_LPM_TIMEOUT_SEC)
    {
        set_screen_power_mode(ST7305_PWR_MODE_LPM);
    }

    if (!s_auto_sleep || !s_deepsleep_idle_detect_enabled)
        return;

    s_idle_seconds++;
    if (s_idle_seconds >= DEEPSLEEP_IDLE_TIMEOUT_SEC)
    {
        ESP_LOGI(TAG, "idle timeout %ds, entering deep sleep", DEEPSLEEP_IDLE_TIMEOUT_SEC);
        power_management_enter_deepsleep(60 * 1000);
    }
}

void power_management_reset_idle_timer(void)
{
    s_idle_seconds = 0;
    s_screen_idle_seconds = 0;
}

void power_management_start_idle_timer(void)
{
    if (s_idle_timer == NULL)
    {
        const esp_timer_create_args_t timer_args = {
            .callback = idle_timer_cb,
            .name = "idle_timer",
            .arg = NULL,
            .skip_unhandled_events = true,
        };
        esp_err_t err = esp_timer_create(&timer_args, &s_idle_timer);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "create idle timer failed: %s", esp_err_to_name(err));
            return;
        }
    }
    power_management_reset_idle_timer();
    esp_err_t err = esp_timer_is_active(s_idle_timer) ? esp_timer_restart(s_idle_timer, 1000000) : esp_timer_start_periodic(s_idle_timer, 1000000);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "start idle timer failed: %s", esp_err_to_name(err));
        return;
    }
    ESP_LOGD(TAG, "idle timer started");
}

void power_management_stop_idle_timer(void)
{
    if (s_idle_timer != NULL)
    {
        esp_timer_stop(s_idle_timer);
        ESP_LOGD(TAG, "idle timer stopped");
    }
    power_management_reset_idle_timer();
}

void power_management_start_deepsleep_idle_detect(void)
{
    s_deepsleep_idle_detect_enabled = true;
    s_idle_seconds = 0;
}

void power_management_stop_deepsleep_idle_detect(void)
{
    s_deepsleep_idle_detect_enabled = false;
    s_idle_seconds = 0;
}

/* ==================== Deep Sleep / Wakeup ==================== */

bool power_management_is_wakeup_from_sleep(void)
{
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    return (cause == ESP_SLEEP_WAKEUP_TIMER) || (cause == ESP_SLEEP_WAKEUP_EXT1);
}

bool power_management_is_wakeup_from_timer(void)
{
    return esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER;
}

bool power_management_is_wakeup_by_touch(void)
{
    if (esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_EXT1)
    {
        return false;
    }

    return (esp_sleep_get_ext1_wakeup_status() & BIT(TOUCH_INT_PIN)) != 0;
}

bool power_management_is_wakeup_by_rtc(void)
{
    if (esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_EXT1)
    {
        return false;
    }

    return (esp_sleep_get_ext1_wakeup_status() & BIT(RTC_INT_PIN)) != 0;
}

esp_err_t power_management_register_pre_deepsleep_cb(power_management_hook_cb_t cb, void *user_data)
{
    s_pre_deepsleep_cb = cb;
    s_pre_deepsleep_user_data = user_data;
    return ESP_OK;
}

static esp_err_t configure_rtc_periodic_wakeup(void)
{
    pcf85263a_handle_t rtc_handle = pcf85263a_get_handle();
    if (rtc_handle == NULL)
    {
        return ESP_ERR_INVALID_STATE;
    }

    ESP_RETURN_ON_ERROR(pcf85263a_set_inta_mode(rtc_handle, PCF85263A_INTA_MODE_INTERRUPT),
                        TAG,
                        "set RTC INTA interrupt mode failed");
    ESP_RETURN_ON_ERROR(pcf85263a_set_inta_mask(rtc_handle, PCF85263A_INTA_ILP, true),
                        TAG,
                        "set RTC INTA level interrupt mode failed");
    ESP_RETURN_ON_ERROR(pcf85263a_enable_alarm1_interrupt(rtc_handle, false),
                        TAG,
                        "disable RTC alarm1 interrupt failed");
    ESP_RETURN_ON_ERROR(pcf85263a_clear_flags(rtc_handle, PCF85263A_FLAG_PIF | PCF85263A_FLAG_A1F),
                        TAG,
                        "clear RTC flags failed");
    ESP_RETURN_ON_ERROR(pcf85263a_set_periodic_interrupt(rtc_handle, PCF85263A_PERIODIC_EVERY_MINUTE),
                        TAG,
                        "set RTC periodic wakeup failed");

    return ESP_OK;
}

static esp_err_t configure_rtc_alarm_wakeup(uint8_t hour, uint8_t minute)
{
    pcf85263a_handle_t rtc_handle = pcf85263a_get_handle();
    if (rtc_handle == NULL)
    {
        return ESP_ERR_INVALID_STATE;
    }

    pcf85263a_datetime_t now = {0};
    esp_err_t err = pcf85263a_get_datetime(rtc_handle, &now);
    if (err != ESP_OK)
    {
        return err;
    }

    pcf85263a_alarm1_t alarm = {
        .second = 0,
        .minute = minute,
        .hour = hour,
        .day = now.day,
        .month = now.month,
        .match_second = true,
        .match_minute = true,
        .match_hour = true,
        .match_day = false,
        .match_month = false,
    };

    ESP_RETURN_ON_ERROR(pcf85263a_set_inta_mode(rtc_handle, PCF85263A_INTA_MODE_INTERRUPT),
                        TAG,
                        "set RTC INTA interrupt mode failed");
    ESP_RETURN_ON_ERROR(pcf85263a_set_inta_mask(rtc_handle, PCF85263A_INTA_ILP, true),
                        TAG,
                        "set RTC INTA level interrupt mode failed");
    ESP_RETURN_ON_ERROR(pcf85263a_set_periodic_interrupt(rtc_handle, PCF85263A_PERIODIC_DISABLED),
                        TAG,
                        "disable RTC periodic wakeup failed");
    ESP_RETURN_ON_ERROR(pcf85263a_set_alarm1(rtc_handle, &alarm),
                        TAG,
                        "set RTC alarm1 wakeup failed");
    ESP_RETURN_ON_ERROR(pcf85263a_clear_flags(rtc_handle, PCF85263A_FLAG_PIF | PCF85263A_FLAG_A1F),
                        TAG,
                        "clear RTC flags failed");
    ESP_RETURN_ON_ERROR(pcf85263a_enable_alarm1_interrupt(rtc_handle, true),
                        TAG,
                        "enable RTC alarm1 interrupt failed");

    ESP_LOGI(TAG, "RTC alarm wakeup configured at %02u:%02u:00", hour, minute);
    return ESP_OK;
}

static void enter_deepsleep_with_ext1_wakeup(const char *rtc_mode_desc)
{
    /* 清掉上一轮 deep sleep 遗留的唤醒源配置/状态，避免下一次入睡时误复用旧 timer。 */
    esp_err_t err = esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
    if (err != ESP_OK)
    {
        ESP_LOGW(TAG, "disable previous wakeup sources failed: %s", esp_err_to_name(err));
    }

    uint64_t wakeup_mask = BIT(TOUCH_INT_PIN) | BIT(RTC_INT_PIN);
    err = esp_sleep_enable_ext1_wakeup(wakeup_mask, ESP_EXT1_WAKEUP_ANY_LOW);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "enable ext1 wakeup failed (GPIO%d/GPIO%d): %s",
                 TOUCH_INT_PIN, RTC_INT_PIN, esp_err_to_name(err));
    }

    ESP_LOGI(TAG, "entering deep sleep, wakeup: RTC %s on GPIO%d low + touch GPIO%d low",
             rtc_mode_desc, RTC_INT_PIN, TOUCH_INT_PIN);
    esp_deep_sleep_start();
}

void power_management_enter_deepsleep(uint32_t wakeup_time_ms)
{
    if (wakeup_time_ms != DEFAULT_RTC_WAKEUP_MS)
    {
        ESP_LOGD(TAG, "timer wakeup request %lu ms is mapped to RTC every-minute wakeup",
                 (unsigned long)wakeup_time_ms);
    }

    /* 停止空闲计时器，避免 deep sleep 期间触发 */
    power_management_stop_idle_timer();
    s_deepsleep_idle_detect_enabled = false;

    /* 外设准备：允许在 deep sleep 前关断耗电外设 */
    if (s_pre_deepsleep_cb != NULL)
    {
        s_pre_deepsleep_cb(s_pre_deepsleep_user_data);
    }

    esp_err_t err = configure_rtc_periodic_wakeup();
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "configure RTC periodic wakeup failed: %s", esp_err_to_name(err));
    }

    enter_deepsleep_with_ext1_wakeup("every minute");
    /* 不会执行到这里 */
}

void power_management_enter_deepsleep_until_rtc_time(uint8_t hour, uint8_t minute)
{
    if (hour > 23)
    {
        hour = 0;
    }
    if (minute > 59)
    {
        minute = 0;
    }

    power_management_stop_idle_timer();
    s_deepsleep_idle_detect_enabled = false;

    if (s_pre_deepsleep_cb != NULL)
    {
        s_pre_deepsleep_cb(s_pre_deepsleep_user_data);
    }

    esp_err_t err = configure_rtc_alarm_wakeup(hour, minute);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "configure RTC alarm wakeup failed: %s", esp_err_to_name(err));
    }

    enter_deepsleep_with_ext1_wakeup("alarm");
    /* 不会执行到这里 */
}

/* ==================== 初始化 ==================== */

esp_err_t power_management_init(void)
{
    /* 从 NVS 读取配置 */
    nvs_read_bool(NVS_KEY_AUTO_LPM, DEFAULT_AUTO_LPM, &s_auto_lightsleep);
    nvs_read_bool(NVS_KEY_AUTO_SLEEP, DEFAULT_AUTO_SLEEP, &s_auto_sleep);
    nvs_read_u8(NVS_KEY_CHG_THR, DEFAULT_CHG_THRESHOLD, &s_charge_threshold);

    ESP_LOGI(TAG, "config from NVS: auto_lpm=%d, auto_sleep=%d, chg_thr=%d%%",
             s_auto_lightsleep, s_auto_sleep, s_charge_threshold);

    /* 如果低功耗模式已开启，立即生效 */
    if (s_auto_lightsleep)
    {
        apply_auto_lightsleep(true);
    }

    s_deepsleep_idle_detect_enabled = false;

    power_management_start_idle_timer();
    return ESP_OK;
}
