#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "esp_log.h"
#include "esp_pm.h"
#include "nvs_flash.h"

#include "cJSON.h"

#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#include "ble.h"
#include "nvs_storage.h"
#include "esp_bt.h"
#include "pcf85263a.h"
#include "nvs.h"
#include "power_management.h"

#define TAG "BLE"

#define POWER_MGMT_NVS_NAMESPACE "power_mgr"
#define NVS_KEY_AUTO_SLEEP "pm_auto_sleep"
#define NVS_KEY_AUTO_LPM "pm_auto_lpm"
#define NVS_KEY_CHG_THR "pm_chg_thr"
#define NVS_KEY_SLEEP_START_HOUR "pm_sleep_sh"
#define NVS_KEY_SLEEP_START_MINUTE "pm_sleep_sm"
#define NVS_KEY_SLEEP_END_HOUR "pm_sleep_eh"
#define NVS_KEY_SLEEP_END_MINUTE "pm_sleep_em"
#define DEFAULT_SLEEP_START_HOUR 22
#define DEFAULT_SLEEP_START_MINUTE 0
#define DEFAULT_SLEEP_END_HOUR 6
#define DEFAULT_SLEEP_END_MINUTE 0
#define DEFAULT_CHARGE_THRESHOLD 90
#define POMODORO_NVS_NAMESPACE "pomodoro"
#define NVS_KEY_FOCUS_MIN "focus_min"
#define NVS_KEY_REST_MIN "rest_min"
#define DEFAULT_FOCUS_MIN 25
#define DEFAULT_REST_MIN 5

/* Device name advertised over BLE */
#define BLE_DEVICE_NAME "FocusTimer"

/* Advertising interval (0.625ms units): 800 = 0.5s */
#define BLE_ADV_INTERVAL_MIN 800
#define BLE_ADV_INTERVAL_MAX 3200

/* Custom 128-bit UUIDs for the FocusTimer service and characteristics */
/* Service:    00000000-0000-0000-0000-0000-0000-FFF0 */
static const ble_uuid128_t focus_timer_svc_uuid =
    BLE_UUID128_INIT(0xF0, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

/* Characteristic 1 (current day data): 00000000-0000-0000-0000-0000-0000-0001 */
static const ble_uuid128_t current_day_chr_uuid =
    BLE_UUID128_INIT(0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

/* Characteristic 2 (history JSON):     00000000-0000-0000-0000-0000-0000-0002 */
static const ble_uuid128_t history_chr_uuid =
    BLE_UUID128_INIT(0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

/* Characteristic 3 (set datetime JSON): 00000000-0000-0000-0000-0000-0000-0003 */
static const ble_uuid128_t datetime_set_chr_uuid =
    BLE_UUID128_INIT(0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

/* Characteristic 4 (trigger send history): 00000000-0000-0000-0000-0000-0000-0004 */
static const ble_uuid128_t send_history_chr_uuid =
    BLE_UUID128_INIT(0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

/* Characteristic 5 (power settings JSON): 00000000-0000-0000-0000-0000-0000-0005 */
static const ble_uuid128_t power_settings_chr_uuid =
    BLE_UUID128_INIT(0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

/* Characteristic 6 (pomodoro duration JSON): 00000000-0000-0000-0000-0000-0000-0006 */
static const ble_uuid128_t pomodoro_settings_chr_uuid =
    BLE_UUID128_INIT(0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

static uint16_t current_day_chr_val_handle;
static uint16_t history_chr_val_handle;
static uint16_t datetime_set_chr_val_handle;
static uint16_t send_history_chr_val_handle;
static uint16_t power_settings_chr_val_handle;
static uint16_t pomodoro_settings_chr_val_handle;

static bool ble_connected;
static uint16_t ble_conn_handle;
static uint8_t own_addr_type;
static bool ble_synced;
static bool ble_adv_enabled;
static bool ble_inited;
static esp_pm_lock_handle_t s_ble_cpu_lock;
static esp_pm_lock_handle_t s_ble_sleep_lock;
static bool s_ble_pm_locked;

/* History notification subscription state */
static bool history_chr_subscribed;

/* Fragmented history send state */
#define BLE_HISTORY_CHUNK_DATA_SIZE 502 //502=MTU(512)-ATT header(3)-自定义帧头7
static char *s_history_json_buf;
static size_t s_history_json_len;
static int s_history_total_chunks;
static int s_history_current_chunk;

static ble_connection_state_cb_t s_conn_state_cb;
static void *s_conn_state_cb_arg;
static ble_datetime_updated_cb_t s_datetime_updated_cb;
static void *s_datetime_updated_cb_arg;
static ble_power_settings_updated_cb_t s_power_settings_updated_cb;
static void *s_power_settings_updated_cb_arg;

typedef struct {
    uint8_t start_hour;
    uint8_t start_minute;
    uint8_t end_hour;
    uint8_t end_minute;
    bool low_power;
    bool auto_sleep;
    uint8_t charge_threshold;
} ble_power_settings_t;

typedef struct {
    uint8_t focus_min;
    uint8_t rest_min;
} ble_pomodoro_settings_t;

static void ble_notify_conn_state(bool connected)
{
    if (s_conn_state_cb != NULL) {
        s_conn_state_cb(connected, s_conn_state_cb_arg);
    }
}

static void ble_notify_datetime_updated(void)
{
    if (s_datetime_updated_cb != NULL) {
        s_datetime_updated_cb(s_datetime_updated_cb_arg);
    }
}

static void ble_notify_power_settings_updated(void)
{
    if (s_power_settings_updated_cb != NULL) {
        s_power_settings_updated_cb(s_power_settings_updated_cb_arg);
    }
}

static esp_err_t ble_pm_lock_init(void)
{
    esp_err_t err;

    if (s_ble_cpu_lock == NULL) {
        err = esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "ble_cpu", &s_ble_cpu_lock);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "create BLE CPU PM lock failed: %s", esp_err_to_name(err));
            return err;
        }
    }

    if (s_ble_sleep_lock == NULL) {
        err = esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "ble_sleep", &s_ble_sleep_lock);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "create BLE sleep PM lock failed: %s", esp_err_to_name(err));
            return err;
        }
    }

    return ESP_OK;
}

static esp_err_t ble_pm_lock_acquire(void)
{
    esp_err_t err;

    if (s_ble_pm_locked) {
        return ESP_OK;
    }

    err = ble_pm_lock_init();
    if (err != ESP_OK) {
        return err;
    }

    err = esp_pm_lock_acquire(s_ble_cpu_lock);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "acquire BLE CPU PM lock failed: %s", esp_err_to_name(err));
        return err;
    }

    err = esp_pm_lock_acquire(s_ble_sleep_lock);
    if (err != ESP_OK) {
        (void)esp_pm_lock_release(s_ble_cpu_lock);
        ESP_LOGE(TAG, "acquire BLE sleep PM lock failed: %s", esp_err_to_name(err));
        return err;
    }

    s_ble_pm_locked = true;
    return ESP_OK;
}

static void ble_pm_lock_release(void)
{
    if (!s_ble_pm_locked) {
        return;
    }

    if (s_ble_sleep_lock != NULL) {
        (void)esp_pm_lock_release(s_ble_sleep_lock);
    }
    if (s_ble_cpu_lock != NULL) {
        (void)esp_pm_lock_release(s_ble_cpu_lock);
    }
    s_ble_pm_locked = false;
}

/* Forward declarations */
static void ble_host_task(void *param);
static void ble_on_sync(void);
static void ble_on_reset(int reason);
static int gap_event_handler(struct ble_gap_event *event, void *arg);
static int gatt_access_cb(uint16_t conn_handle, uint16_t attr_handle,
                          struct ble_gatt_access_ctxt *ctxt, void *arg);
static int ble_handle_datetime_write(struct ble_gatt_access_ctxt *ctxt);
static int ble_handle_send_history_write(struct ble_gatt_access_ctxt *ctxt);
static int ble_handle_power_settings_write(struct ble_gatt_access_ctxt *ctxt);
static int ble_handle_pomodoro_settings_write(struct ble_gatt_access_ctxt *ctxt);
static esp_err_t ble_parse_datetime_json(const char *json, pcf85263a_datetime_t *datetime);
static esp_err_t ble_parse_power_settings_json(const char *json, ble_power_settings_t *settings);
static esp_err_t ble_parse_pomodoro_settings_json(const char *json, ble_pomodoro_settings_t *settings);
static esp_err_t ble_get_power_settings_json(char *json_buf, size_t json_buf_size, size_t *json_len);
static esp_err_t ble_get_pomodoro_settings_json(char *json_buf, size_t json_buf_size, size_t *json_len);
void ble_store_config_init(void);

static bool ble_json_get_int(const cJSON *object, const char *name, int *value)
{
    const cJSON *item = cJSON_GetObjectItemCaseSensitive(object, name);
    if (!cJSON_IsNumber(item)) {
        return false;
    }

    *value = item->valueint;
    return true;
}

static bool ble_json_get_string(const cJSON *object, const char *name, const char **value)
{
    const cJSON *item = cJSON_GetObjectItemCaseSensitive(object, name);
    if (!cJSON_IsString(item) || item->valuestring == NULL) {
        return false;
    }

    *value = item->valuestring;
    return true;
}

static uint8_t ble_nvs_read_u8_default_from(const char *namespace_name, const char *key, uint8_t default_val);

static uint8_t ble_nvs_read_u8_default(const char *key, uint8_t default_val)
{
    return ble_nvs_read_u8_default_from(POWER_MGMT_NVS_NAMESPACE, key, default_val);
}

static uint8_t ble_nvs_read_u8_default_from(const char *namespace_name, const char *key, uint8_t default_val)
{
    nvs_handle_t handle;
    uint8_t value = default_val;

    if (nvs_open(namespace_name, NVS_READONLY, &handle) == ESP_OK) {
        if (nvs_get_u8(handle, key, &value) != ESP_OK) {
            value = default_val;
        }
        nvs_close(handle);
    }

    return value;
}

static esp_err_t ble_nvs_write_pomodoro_settings(const ble_pomodoro_settings_t *settings)
{
    nvs_handle_t handle;

    if (settings == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = nvs_open(POMODORO_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return err;
    }

    err = nvs_set_u8(handle, NVS_KEY_FOCUS_MIN, settings->focus_min);
    err |= nvs_set_u8(handle, NVS_KEY_REST_MIN, settings->rest_min);
    err |= nvs_commit(handle);
    nvs_close(handle);
    return err;
}

static bool ble_parse_hhmm_string(const char *value, uint8_t *hour, uint8_t *minute)
{
    if (value == NULL || hour == NULL || minute == NULL || strlen(value) != 4U) {
        return false;
    }

    if (value[0] < '0' || value[0] > '9' ||
        value[1] < '0' || value[1] > '9' ||
        value[2] < '0' || value[2] > '9' ||
        value[3] < '0' || value[3] > '9') {
        return false;
    }

    uint8_t parsed_hour = (uint8_t)((value[0] - '0') * 10 + (value[1] - '0'));
    uint8_t parsed_minute = (uint8_t)((value[2] - '0') * 10 + (value[3] - '0'));

    if (parsed_hour > 23 || parsed_minute > 59) {
        return false;
    }

    *hour = parsed_hour;
    *minute = parsed_minute;
    return true;
}

static esp_err_t ble_parse_power_settings_json(const char *json, ble_power_settings_t *settings)
{
    cJSON *root = NULL;
    const char *start_time = "0000";
    const char *stop_time = "0000";
    uint8_t start_hour = 0;
    uint8_t start_minute = 0;
    uint8_t stop_hour = 0;
    uint8_t stop_minute = 0;
    int low_power = 0;
    int auto_sleep = 0;
    int charge_threshold = 0;

    if (json == NULL || settings == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    root = cJSON_Parse(json);
    if (root == NULL) {
        ESP_LOGW(TAG, "Invalid power settings JSON");
        return ESP_ERR_INVALID_ARG;
    }

    if (!ble_json_get_string(root, "start_time", &start_time) ||
        !ble_json_get_string(root, "stop_time", &stop_time)||
        !ble_json_get_int(root, "low_power", &low_power) ||
        !ble_json_get_int(root, "auto_sleep", &auto_sleep) ||
        !ble_json_get_int(root, "charge_thresh", &charge_threshold)) {
        cJSON_Delete(root);
        ESP_LOGW(TAG, "Power settings JSON missing required fields");
        return ESP_ERR_INVALID_ARG;
    }

    if (!ble_parse_hhmm_string(start_time, &start_hour, &start_minute) ||
        !ble_parse_hhmm_string(stop_time, &stop_hour, &stop_minute)) {
        cJSON_Delete(root);
        ESP_LOGW(TAG, "Power settings time value out of range");
        return ESP_ERR_INVALID_ARG;
    }

    cJSON_Delete(root);

    if ((low_power != 0 && low_power != 1) ||
        (auto_sleep != 0 && auto_sleep != 1) ||
        charge_threshold < 0 || charge_threshold > 100) {
        ESP_LOGW(TAG, "Power settings value out of range");
        return ESP_ERR_INVALID_ARG;
    }

    settings->start_hour = start_hour;
    settings->start_minute = start_minute;
    settings->end_hour = stop_hour;
    settings->end_minute = stop_minute;
    settings->low_power = low_power != 0;
    settings->auto_sleep = auto_sleep != 0;
    settings->charge_threshold = (uint8_t)charge_threshold;
    return ESP_OK;
}

static esp_err_t ble_get_power_settings_json(char *json_buf, size_t json_buf_size, size_t *json_len)
{
    ble_power_settings_t settings = {
        .start_hour = ble_nvs_read_u8_default(NVS_KEY_SLEEP_START_HOUR, DEFAULT_SLEEP_START_HOUR),
        .start_minute = ble_nvs_read_u8_default(NVS_KEY_SLEEP_START_MINUTE, DEFAULT_SLEEP_START_MINUTE),
        .end_hour = ble_nvs_read_u8_default(NVS_KEY_SLEEP_END_HOUR, DEFAULT_SLEEP_END_HOUR),
        .end_minute = ble_nvs_read_u8_default(NVS_KEY_SLEEP_END_MINUTE, DEFAULT_SLEEP_END_MINUTE),
        .low_power = power_management_get_auto_lightsleep(),
        .auto_sleep = power_management_get_auto_sleep(),
        .charge_threshold = power_management_get_charge_threshold(),
    };
    int written = 0;

    if (json_buf == NULL || json_len == NULL || json_buf_size == 0U) {
        return ESP_ERR_INVALID_ARG;
    }

    if (settings.start_hour > 23) settings.start_hour = 0;
    if (settings.start_minute > 59) settings.start_minute = 0;
    if (settings.end_hour > 23) settings.end_hour = 0;
    if (settings.end_minute > 59) settings.end_minute = 0;

    written = snprintf(json_buf,
                       json_buf_size,
                       "{\"start_time\":\"%02u%02u\",\"stop_time\":\"%02u%02u\",\"low_power\":%u,\"auto_sleep\":%u,\"charge_thresh\":%u}",
                       settings.start_hour,
                       settings.start_minute,
                       settings.end_hour,
                       settings.end_minute,
                       settings.low_power ? 1U : 0U,
                       settings.auto_sleep ? 1U : 0U,
                       settings.charge_threshold);
    if (written < 0 || (size_t)written >= json_buf_size) {
        return ESP_ERR_NO_MEM;
    }

    *json_len = (size_t)written;
    return ESP_OK;
}

static esp_err_t ble_parse_pomodoro_settings_json(const char *json, ble_pomodoro_settings_t *settings)
{
    cJSON *root = NULL;
    int focus_min = 0;
    int rest_min = 0;

    if (json == NULL || settings == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    root = cJSON_Parse(json);
    if (root == NULL) {
        ESP_LOGW(TAG, "Invalid pomodoro settings JSON");
        return ESP_ERR_INVALID_ARG;
    }

    if (!ble_json_get_int(root, "focus_min", &focus_min) ||
        !ble_json_get_int(root, "rest_min", &rest_min)) {
        cJSON_Delete(root);
        ESP_LOGW(TAG, "Pomodoro settings JSON missing required fields");
        return ESP_ERR_INVALID_ARG;
    }

    cJSON_Delete(root);

    if (focus_min <= 0 || focus_min > UINT8_MAX ||
        rest_min <= 0 || rest_min > UINT8_MAX) {
        ESP_LOGW(TAG, "Pomodoro settings value out of range");
        return ESP_ERR_INVALID_ARG;
    }

    settings->focus_min = (uint8_t)focus_min;
    settings->rest_min = (uint8_t)rest_min;
    return ESP_OK;
}

static esp_err_t ble_get_pomodoro_settings_json(char *json_buf, size_t json_buf_size, size_t *json_len)
{
    ble_pomodoro_settings_t settings = {
        .focus_min = ble_nvs_read_u8_default_from(POMODORO_NVS_NAMESPACE,
                                                  NVS_KEY_FOCUS_MIN,
                                                  DEFAULT_FOCUS_MIN),
        .rest_min = ble_nvs_read_u8_default_from(POMODORO_NVS_NAMESPACE,
                                                 NVS_KEY_REST_MIN,
                                                 DEFAULT_REST_MIN),
    };
    int written = 0;

    if (json_buf == NULL || json_len == NULL || json_buf_size == 0U) {
        return ESP_ERR_INVALID_ARG;
    }

    if (settings.focus_min == 0U) {
        settings.focus_min = DEFAULT_FOCUS_MIN;
    }
    if (settings.rest_min == 0U) {
        settings.rest_min = DEFAULT_REST_MIN;
    }

    written = snprintf(json_buf,
                       json_buf_size,
                       "{\"focus_min\":%u,\"rest_min\":%u}",
                       settings.focus_min,
                       settings.rest_min);
    if (written < 0 || (size_t)written >= json_buf_size) {
        return ESP_ERR_NO_MEM;
    }

    *json_len = (size_t)written;
    return ESP_OK;
}

static esp_err_t ble_parse_datetime_json(const char *json, pcf85263a_datetime_t *datetime)
{
    cJSON *root = cJSON_Parse(json);
    if (root == NULL) {
        ESP_LOGW(TAG, "Invalid datetime JSON");
        return ESP_ERR_INVALID_ARG;
    }

    const cJSON *date = cJSON_GetObjectItemCaseSensitive(root, "date");
    const cJSON *time = cJSON_GetObjectItemCaseSensitive(root, "time");
    const cJSON *weekday = cJSON_GetObjectItemCaseSensitive(root, "weekday");
    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int minute = 0;
    int second = 0;
    int weekday_index = 0;

    if (!cJSON_IsObject(date) ||
        !ble_json_get_int(date, "year", &year) ||
        !ble_json_get_int(date, "month", &month) ||
        !ble_json_get_int(date, "day", &day) ||
        !cJSON_IsObject(time) ||
        !ble_json_get_int(time, "hour", &hour) ||
        !ble_json_get_int(time, "minute", &minute) ||
        !ble_json_get_int(time, "second", &second) ||
        !cJSON_IsObject(weekday) ||
        !ble_json_get_int(weekday, "index", &weekday_index)) {
        cJSON_Delete(root);
        ESP_LOGW(TAG, "Datetime JSON missing required fields");
        return ESP_ERR_INVALID_ARG;
    }

    cJSON_Delete(root);

    if (year < 2000 || year > 2099 ||
        month < 1 || month > 12 ||
        day < 1 || day > 31 ||
        hour < 0 || hour > 23 ||
        minute < 0 || minute > 59 ||
        second < 0 || second > 59 ||
        weekday_index < 0 || weekday_index > 6) {
        ESP_LOGW(TAG, "Datetime JSON value out of range");
        return ESP_ERR_INVALID_ARG;
    }

    datetime->year = (uint16_t)year;
    datetime->month = (uint8_t)month;
    datetime->day = (uint8_t)day;
    datetime->hour = (uint8_t)hour;
    datetime->minute = (uint8_t)minute;
    datetime->second = (uint8_t)second;
    datetime->weekday = (uint8_t)weekday_index;
    return ESP_OK;
}

static int ble_handle_datetime_write(struct ble_gatt_access_ctxt *ctxt)
{
    uint16_t payload_len = OS_MBUF_PKTLEN(ctxt->om);
    char *json_buf = NULL;
    pcf85263a_datetime_t datetime = {0};
    pcf85263a_handle_t rtc_handle = pcf85263a_get_handle();

    if (rtc_handle == NULL) {
        ESP_LOGE(TAG, "RTC handle unavailable");
        return BLE_ATT_ERR_UNLIKELY;
    }

    if (payload_len == 0U) {
        return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }

    json_buf = calloc((size_t)payload_len + 1U, sizeof(char));
    if (json_buf == NULL) {
        ESP_LOGE(TAG, "Failed to allocate datetime JSON buffer");
        return BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    int rc = os_mbuf_copydata(ctxt->om, 0, payload_len, json_buf);
    if (rc != 0) {
        free(json_buf);
        ESP_LOGE(TAG, "Failed to copy datetime payload: %d", rc);
        return BLE_ATT_ERR_UNLIKELY;
    }

    esp_err_t err = ble_parse_datetime_json(json_buf, &datetime);
    free(json_buf);
    if (err != ESP_OK) {
        return BLE_ATT_ERR_INVALID_PDU;
    }

    err = pcf85263a_set_datetime(rtc_handle, &datetime);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set RTC datetime: %s", esp_err_to_name(err));
        return BLE_ATT_ERR_UNLIKELY;
    }

    ble_notify_datetime_updated();

    ESP_LOGI(TAG, "RTC datetime updated over BLE: %u-%02u-%02u %02u:%02u:%02u weekday=%u",
             datetime.year, datetime.month, datetime.day,
             datetime.hour, datetime.minute, datetime.second, datetime.weekday);
    return 0;
}

static int ble_handle_power_settings_write(struct ble_gatt_access_ctxt *ctxt)
{
    uint16_t payload_len = OS_MBUF_PKTLEN(ctxt->om);
    char *json_buf = NULL;
    ble_power_settings_t settings = {0};
    nvs_handle_t handle;

    if (payload_len == 0U) {
        return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }

    json_buf = calloc((size_t)payload_len + 1U, sizeof(char));
    if (json_buf == NULL) {
        ESP_LOGE(TAG, "Failed to allocate power settings JSON buffer");
        return BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    int rc = os_mbuf_copydata(ctxt->om, 0, payload_len, json_buf);
    if (rc != 0) {
        free(json_buf);
        ESP_LOGE(TAG, "Failed to copy power settings payload: %d", rc);
        return BLE_ATT_ERR_UNLIKELY;
    }

    esp_err_t err = ble_parse_power_settings_json(json_buf, &settings);
    free(json_buf);
    if (err != ESP_OK) {
        return BLE_ATT_ERR_INVALID_PDU;
    }

    err = nvs_open(POWER_MGMT_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open power settings NVS: %s", esp_err_to_name(err));
        return BLE_ATT_ERR_UNLIKELY;
    }

    err = nvs_set_u8(handle, NVS_KEY_AUTO_LPM, settings.low_power ? 1U : 0U);
    err |= nvs_set_u8(handle, NVS_KEY_AUTO_SLEEP, settings.auto_sleep ? 1U : 0U);
    err |= nvs_set_u8(handle, NVS_KEY_CHG_THR, settings.charge_threshold);
    err |= nvs_set_u8(handle, NVS_KEY_SLEEP_START_HOUR, settings.start_hour);
    err |= nvs_set_u8(handle, NVS_KEY_SLEEP_START_MINUTE, settings.start_minute);
    err |= nvs_set_u8(handle, NVS_KEY_SLEEP_END_HOUR, settings.end_hour);
    err |= nvs_set_u8(handle, NVS_KEY_SLEEP_END_MINUTE, settings.end_minute);
    err |= nvs_commit(handle);
    nvs_close(handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to persist power settings: %s", esp_err_to_name(err));
        return BLE_ATT_ERR_UNLIKELY;
    }

    (void)power_management_set_auto_lightsleep(settings.low_power);
    (void)power_management_set_auto_sleep(settings.auto_sleep);
    (void)power_management_set_charge_threshold(settings.charge_threshold);
    ble_notify_power_settings_updated();

    ESP_LOGI(TAG,
             "Power settings updated over BLE: low_power=%u auto_sleep=%u charge_thresh=%u sleep_period=%02u:%02u-%02u:%02u",
             settings.low_power ? 1U : 0U,
             settings.auto_sleep ? 1U : 0U,
             settings.charge_threshold,
             settings.start_hour,
             settings.start_minute,
             settings.end_hour,
             settings.end_minute);
    return 0;
}

static int ble_handle_pomodoro_settings_write(struct ble_gatt_access_ctxt *ctxt)
{
    uint16_t payload_len = OS_MBUF_PKTLEN(ctxt->om);
    char *json_buf = NULL;
    ble_pomodoro_settings_t settings = {0};

    if (payload_len == 0U) {
        return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }

    json_buf = calloc((size_t)payload_len + 1U, sizeof(char));
    if (json_buf == NULL) {
        ESP_LOGE(TAG, "Failed to allocate pomodoro settings JSON buffer");
        return BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    int rc = os_mbuf_copydata(ctxt->om, 0, payload_len, json_buf);
    if (rc != 0) {
        free(json_buf);
        ESP_LOGE(TAG, "Failed to copy pomodoro settings payload: %d", rc);
        return BLE_ATT_ERR_UNLIKELY;
    }

    esp_err_t err = ble_parse_pomodoro_settings_json(json_buf, &settings);
    free(json_buf);
    if (err != ESP_OK) {
        return BLE_ATT_ERR_INVALID_PDU;
    }

    err = ble_nvs_write_pomodoro_settings(&settings);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to persist pomodoro settings: %s", esp_err_to_name(err));
        return BLE_ATT_ERR_UNLIKELY;
    }

    ESP_LOGI(TAG,
             "Pomodoro settings updated over BLE: focus_min=%u rest_min=%u",
             settings.focus_min,
             settings.rest_min);
    return 0;
}

/*** Fragmented history data send via notifications ***/

static void ble_send_history_fragments(void)
{
    if (!ble_connected || !history_chr_subscribed) {
        ESP_LOGW(TAG, "Cannot send history: not connected or not subscribed");
        free(s_history_json_buf);
        s_history_json_buf = NULL;
        return;
    }

    if (s_history_json_len == 0) {
        ESP_LOGW(TAG, "History JSON is empty, nothing to send");
        free(s_history_json_buf);
        s_history_json_buf = NULL;
        return;
    }

    s_history_total_chunks = (int)((s_history_json_len + BLE_HISTORY_CHUNK_DATA_SIZE - 1) / BLE_HISTORY_CHUNK_DATA_SIZE);
    s_history_current_chunk = 0;

    ESP_LOGI(TAG, "Sending history: %zu bytes, %d chunks", s_history_json_len, s_history_total_chunks);

    for (int i = 0; i < s_history_total_chunks; i++) {
        size_t offset = (size_t)i * BLE_HISTORY_CHUNK_DATA_SIZE;
        size_t chunk_data_len = s_history_json_len - offset;
        if (chunk_data_len > BLE_HISTORY_CHUNK_DATA_SIZE) {
            chunk_data_len = BLE_HISTORY_CHUNK_DATA_SIZE;
        }

        /* Frame format: "(n/m)xxx" — n and m are 1-based */
        int frame_len = snprintf(NULL, 0, "(%02d/%02d)", i + 1, s_history_total_chunks);
        size_t total_len = (size_t)frame_len + chunk_data_len;

        char *frame_buf = malloc(total_len);
        if (frame_buf == NULL) {
            ESP_LOGE(TAG, "Failed to allocate frame buffer for chunk %d", i + 1);
            break;
        }
        snprintf(frame_buf, (size_t)frame_len + 1, "(%02d/%02d)", i + 1, s_history_total_chunks);
        memcpy(frame_buf + frame_len, s_history_json_buf + offset, chunk_data_len);

        struct os_mbuf *om = ble_hs_mbuf_from_flat(frame_buf, total_len);
        if (om == NULL) {
            ESP_LOGE(TAG, "Failed to create mbuf for chunk %d", i + 1);
            free(frame_buf);
            break;
        }
        int rc = ble_gatts_notify_custom(ble_conn_handle, history_chr_val_handle, om);
        free(frame_buf);
        if (rc != 0) {
            ESP_LOGE(TAG, "Failed to notify chunk %d/%d: %d", i + 1, s_history_total_chunks, rc);
            break;
        }
        s_history_current_chunk = i + 1;
    }

    ESP_LOGI(TAG, "History send complete: %d/%d chunks sent", s_history_current_chunk, s_history_total_chunks);
    free(s_history_json_buf);
    s_history_json_buf = NULL;
    s_history_json_len = 0;
}

static int ble_handle_send_history_write(struct ble_gatt_access_ctxt *ctxt)
{
    if (!ble_connected) {
        ESP_LOGW(TAG, "Cannot send history: not connected");
        return BLE_ATT_ERR_UNLIKELY;
    }

    if (!history_chr_subscribed) {
        ESP_LOGW(TAG, "Cannot send history: APP not subscribed to history characteristic");
        return BLE_ATT_ERR_UNLIKELY;
    }

    /* Free any previous incomplete transfer */
    free(s_history_json_buf);
    s_history_json_buf = NULL;

    s_history_json_buf = malloc(4096);
    if (s_history_json_buf == NULL) {
        ESP_LOGE(TAG, "Failed to allocate history JSON buffer");
        return BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    esp_err_t err = nvs_storage_get_records_json(s_history_json_buf, 4096, &s_history_json_len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get records JSON: %s", esp_err_to_name(err));
        free(s_history_json_buf);
        s_history_json_buf = NULL;
        return BLE_ATT_ERR_UNLIKELY;
    }

    ble_send_history_fragments();
    return 0;
}

/*** GATT service definition ***/
static const struct ble_gatt_svc_def gatt_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &focus_timer_svc_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]) {
            {
                /* Current day totals (6-byte binary) */
                .uuid = &current_day_chr_uuid.u,
                .access_cb = gatt_access_cb,
                .flags = BLE_GATT_CHR_F_READ,
                .val_handle = &current_day_chr_val_handle,
            }, {
                /* History records JSON (notify for fragmented transfer) */
                .uuid = &history_chr_uuid.u,
                .access_cb = gatt_access_cb,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                .val_handle = &history_chr_val_handle,
            }, {
                /* Set RTC datetime JSON */
                .uuid = &datetime_set_chr_uuid.u,
                .access_cb = gatt_access_cb,
                .flags = BLE_GATT_CHR_F_WRITE,
                .val_handle = &datetime_set_chr_val_handle,
            }, {
                /* Trigger send history (write any data to start fragmented transfer) */
                .uuid = &send_history_chr_uuid.u,
                .access_cb = gatt_access_cb,
                .flags = BLE_GATT_CHR_F_WRITE,
                .val_handle = &send_history_chr_val_handle,
            }, {
                /* Power settings JSON */
                .uuid = &power_settings_chr_uuid.u,
                .access_cb = gatt_access_cb,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
                .val_handle = &power_settings_chr_val_handle,
            }, {
                /* Pomodoro duration settings JSON */
                .uuid = &pomodoro_settings_chr_uuid.u,
                .access_cb = gatt_access_cb,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
                .val_handle = &pomodoro_settings_chr_val_handle,
            }, {
                0, /* Terminator */
            },
        },
    },
    {
        0, /* Terminator */
    },
};

/*** GATT access callback — will be fully implemented in Stage 2 ***/
static int gatt_access_cb(uint16_t conn_handle, uint16_t attr_handle,
                          struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    int rc;

    switch (ctxt->op) {
    case BLE_GATT_ACCESS_OP_READ_CHR:
        ESP_LOGD(TAG, "GATT read: conn_handle=%d attr_handle=%d",
                 conn_handle, attr_handle);

        if (attr_handle == current_day_chr_val_handle) {
            nvs_storage_daily_totals_t totals;
            esp_err_t err = nvs_storage_get_daily_totals(&totals);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Failed to get daily totals: %s", esp_err_to_name(err));
                return BLE_ATT_ERR_UNLIKELY;
            }
            rc = os_mbuf_append(ctxt->om, &totals, sizeof(totals));
            return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
        }

        if (attr_handle == history_chr_val_handle) {
            /* Read returns current JSON (small enough to fit in one read) */
            char *json_buf = malloc(2048);
            if (json_buf == NULL) {
                ESP_LOGE(TAG, "Failed to allocate JSON buffer");
                return BLE_ATT_ERR_INSUFFICIENT_RES;
            }
            size_t json_len = 0;
            esp_err_t err = nvs_storage_get_records_json(json_buf, 2048, &json_len);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Failed to get records JSON: %s", esp_err_to_name(err));
                free(json_buf);
                return BLE_ATT_ERR_UNLIKELY;
            }
            rc = os_mbuf_append(ctxt->om, json_buf, json_len);
            free(json_buf);
            return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
        }

        if (attr_handle == power_settings_chr_val_handle) {
            char json_buf[160];
            size_t json_len = 0;
            esp_err_t err = ble_get_power_settings_json(json_buf, sizeof(json_buf), &json_len);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Failed to get power settings JSON: %s", esp_err_to_name(err));
                return BLE_ATT_ERR_UNLIKELY;
            }
            rc = os_mbuf_append(ctxt->om, json_buf, json_len);
            return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
        }

        if (attr_handle == pomodoro_settings_chr_val_handle) {
            char json_buf[48];
            size_t json_len = 0;
            esp_err_t err = ble_get_pomodoro_settings_json(json_buf, sizeof(json_buf), &json_len);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Failed to get pomodoro settings JSON: %s", esp_err_to_name(err));
                return BLE_ATT_ERR_UNLIKELY;
            }
            rc = os_mbuf_append(ctxt->om, json_buf, json_len);
            return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
        }
        break;

    case BLE_GATT_ACCESS_OP_WRITE_CHR:
        ESP_LOGD(TAG, "GATT write: conn_handle=%d attr_handle=%d",
                 conn_handle, attr_handle);

        if (attr_handle == datetime_set_chr_val_handle) {
            return ble_handle_datetime_write(ctxt);
        }

        if (attr_handle == send_history_chr_val_handle) {
            return ble_handle_send_history_write(ctxt);
        }

        if (attr_handle == power_settings_chr_val_handle) {
            return ble_handle_power_settings_write(ctxt);
        }

        if (attr_handle == pomodoro_settings_chr_val_handle) {
            return ble_handle_pomodoro_settings_write(ctxt);
        }
        break;

    default:
        break;
    }

    return BLE_ATT_ERR_ATTR_NOT_FOUND;
}

/*** GAP event handler ***/
static int gap_event_handler(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        ESP_LOGD(TAG, "BLE_GAP_EVENT_CONNECT; status=%d",
                 event->connect.status);
        if (event->connect.status == 0) {
            ble_connected = true;
            ble_conn_handle = event->connect.conn_handle;
            ble_notify_conn_state(true);
        } else {
            /* Connection failed; resume advertising */
            ble_connected = false;
            ble_notify_conn_state(false);
            if (ble_adv_enabled) {
                ble_start_advertising();
            }
        }
        break;

    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGD(TAG, "BLE_GAP_EVENT_DISCONNECT; reason=%d",
                 event->disconnect.reason);
        ble_connected = false;
        history_chr_subscribed = false;
        if (s_history_json_buf) {
            free(s_history_json_buf);
            s_history_json_buf = NULL;
        }
        ble_notify_conn_state(false);
        if (ble_adv_enabled) {
            ble_start_advertising();
        }
        break;

    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGD(TAG, "BLE_GAP_EVENT_ADV_COMPLETE; reason=%d",
                 event->adv_complete.reason);
        break;

    case BLE_GAP_EVENT_MTU:
        ESP_LOGD(TAG, "MTU update: conn_handle=%d mtu=%d",
                 event->mtu.conn_handle, event->mtu.value);
        break;

    case BLE_GAP_EVENT_SUBSCRIBE:
        ESP_LOGD(TAG, "Subscribe event: attr_handle=%d cur_notify=%d",
                 event->subscribe.attr_handle,
                 event->subscribe.cur_notify);
        if (event->subscribe.attr_handle == history_chr_val_handle) {
            history_chr_subscribed = event->subscribe.cur_notify;
            ESP_LOGI(TAG, "History characteristic %s",
                     history_chr_subscribed ? "subscribed" : "unsubscribed");
        }
        break;

    default:
        break;
    }
    return 0;
}

/*** BLE sync callback — called when controller is ready ***/
static void ble_on_sync(void)
{
    int rc;

    /* Make sure we have a proper address (public for ESP32-H2) */
    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to infer address type: %d", rc);
        return;
    }

    ble_synced = true;
    ESP_LOGI(TAG, "BLE synced");

    if (ble_adv_enabled) {
        ESP_LOGI(TAG, "BLE advertising enabled, starting advertising");
        ble_start_advertising();
    }
}

/*** BLE reset callback ***/
static void ble_on_reset(int reason)
{
    ESP_LOGE(TAG, "BLE host reset; reason=%d", reason);
    ble_connected = false;
    ble_synced = false;
    ble_notify_conn_state(false);
}

/*** NimBLE host task ***/
static void ble_host_task(void *param)
{
    ESP_LOGI(TAG, "BLE host task started");
    nimble_port_run();
    nimble_port_freertos_deinit();
}

/*** Public API ***/

esp_err_t ble_init(void)
{
    int rc;

    if (ble_inited) {
        return ESP_OK;
    }

    ble_connected = false;
    ble_synced = false;
    ble_conn_handle = 0;

    rc = nimble_port_init();
    if (rc != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init NimBLE: %d", rc);
        return rc;
    }

    /* Configure BLE host */
    ble_hs_cfg.reset_cb = ble_on_reset;
    ble_hs_cfg.sync_cb = ble_on_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    /* No security/pairing needed for this device */
    ble_hs_cfg.sm_io_cap = BLE_SM_IO_CAP_NO_IO;
    ble_hs_cfg.sm_bonding = 0;
    ble_hs_cfg.sm_mitm = 0;
    ble_hs_cfg.sm_sc = 0;

    /* Register GATT services */
    rc = ble_gatts_count_cfg(gatt_svcs);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to count GATT config: %d", rc);
        return ESP_FAIL;
    }

    rc = ble_gatts_add_svcs(gatt_svcs);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to add GATT services: %d", rc);
        return ESP_FAIL;
    }

    /* Set device name */
    rc = ble_svc_gap_device_name_set(BLE_DEVICE_NAME);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to set device name: %d", rc);
        return ESP_FAIL;
    }

    /* Initialize BLE store (for bonding data, even though we don't bond) */
    ble_store_config_init();

    /* Start NimBLE host task */
    nimble_port_freertos_init(ble_host_task);

    ESP_LOGI(TAG, "BLE init OK");
    ble_inited = true;
    return ESP_OK;
}

esp_err_t ble_start_advertising(void)
{
    struct ble_gap_adv_params adv_params;
    struct ble_hs_adv_fields adv_fields;
    struct ble_hs_adv_fields rsp_fields;
    int rc;

    if (!ble_inited) {
        return ESP_ERR_INVALID_STATE;
    }

    if (!ble_synced) {
        ESP_LOGW(TAG, "BLE not synced yet, cannot start advertising");
        return ESP_ERR_INVALID_STATE;
    }

    if (ble_gap_adv_active()) {
        return ESP_OK;
    }

    /* --- Advertising data: flags + 128-bit service UUID (fits in 31 bytes) --- */
    memset(&adv_fields, 0, sizeof(adv_fields));
    adv_fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    adv_fields.uuids128 = &focus_timer_svc_uuid;
    adv_fields.num_uuids128 = 1;
    adv_fields.uuids128_is_complete = 1;

    rc = ble_gap_adv_set_fields(&adv_fields);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to set adv fields: %d", rc);
        return ESP_FAIL;
    }

    /* --- Scan response data: complete device name --- */
    memset(&rsp_fields, 0, sizeof(rsp_fields));
    const char *name = ble_svc_gap_device_name();
    rsp_fields.name = (uint8_t *)name;
    rsp_fields.name_len = strlen(name);
    rsp_fields.name_is_complete = 1;

    rc = ble_gap_adv_rsp_set_fields(&rsp_fields);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to set scan rsp fields: %d", rc);
        return ESP_FAIL;
    }

    /* Undirected connectable, general discoverable */
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    adv_params.itvl_min = BLE_ADV_INTERVAL_MIN;
    adv_params.itvl_max = BLE_ADV_INTERVAL_MAX;

    rc = ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER,
                           &adv_params, gap_event_handler, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to start advertising: %d", rc);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "BLE advertising started");
    return ESP_OK;
}

esp_err_t ble_stop(void)
{
    int rc;

    if (!ble_inited) {
        ble_connected = false;
        return ESP_OK;
    }

    if (!ble_synced) {
        ble_connected = false;
        return ESP_OK;
    }

    if (ble_connected) {
        rc = ble_gap_terminate(ble_conn_handle, BLE_ERR_REM_USER_CONN_TERM);
        if (rc != 0) {
            ESP_LOGW(TAG, "Failed to terminate connection: %d", rc);
        }
        ble_connected = false;
    }

    if (ble_gap_adv_active()) {
        rc = ble_gap_adv_stop();
        if (rc != 0) {
            ESP_LOGW(TAG, "Failed to stop advertising: %d", rc);
            return ESP_FAIL;
        }
        ESP_LOGI(TAG, "BLE advertising stopped");
    }

    return ESP_OK;
}

esp_err_t ble_set_advertising_enabled(bool enabled)
{
    if (!enabled) {
        ble_adv_enabled = false;

        esp_err_t err = ble_stop();
        ble_pm_lock_release();
        return err;
    }

    esp_err_t err = ble_pm_lock_acquire();
    if (err != ESP_OK) {
        ble_adv_enabled = false;
        return err;
    }

    ble_adv_enabled = true;

    if (!ble_inited) {
        err = ble_init();
        if (err != ESP_OK) {
            ble_adv_enabled = false;
            ble_pm_lock_release();
            return err;
        }
    }

    if (!ble_synced) {
        // Will be started from ble_on_sync()
        return ESP_OK;
    }

    if (!ble_connected) {
        err = ble_start_advertising();
        if (err != ESP_OK) {
            ble_adv_enabled = false;
            ble_pm_lock_release();
        }
        return err;
    }

    return ESP_OK;
}

bool ble_is_advertising_enabled(void)
{
    return ble_adv_enabled;
}

void ble_register_connection_state_cb(ble_connection_state_cb_t cb, void *arg)
{
    s_conn_state_cb = cb;
    s_conn_state_cb_arg = arg;
}

void ble_register_datetime_updated_cb(ble_datetime_updated_cb_t cb, void *arg)
{
    s_datetime_updated_cb = cb;
    s_datetime_updated_cb_arg = arg;
}

void ble_register_power_settings_updated_cb(ble_power_settings_updated_cb_t cb, void *arg)
{
    s_power_settings_updated_cb = cb;
    s_power_settings_updated_cb_arg = arg;
}

bool ble_is_connected(void)
{
    return ble_connected;
}
