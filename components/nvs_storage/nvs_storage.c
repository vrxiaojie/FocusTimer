#include "nvs_storage.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "nvs.h"
#include "pcf85263a.h"

#define NVS_STORAGE_NAMESPACE "pomodoro"
#define NVS_STORAGE_KEY_RECORDS "daily_records"
#define NVS_STORAGE_DATE_STR_LEN 11
#define NVS_STORAGE_MAX_RECORDS 30
#define NVS_STORAGE_EMPTY_JSON "{\"records\":[]}"

static const char *TAG = "nvs_storage";

typedef struct
{
    bool initialized;
    char current_date[NVS_STORAGE_DATE_STR_LEN];
    nvs_storage_daily_totals_t totals;
} nvs_storage_state_t;

static portMUX_TYPE s_nvs_storage_lock = portMUX_INITIALIZER_UNLOCKED;
static nvs_storage_state_t s_nvs_storage_state = {
    .initialized = false,
    .current_date = {0},
    .totals = {0},
};

static bool nvs_storage_totals_is_empty(const nvs_storage_daily_totals_t *totals)
{
    return totals->focus_minutes == 0 && totals->rest_minutes == 0 &&
           totals->focus_count == 0 && totals->nap_count == 0;
}

static esp_err_t nvs_storage_alloc_empty_json(char **out_json)
{
    size_t json_len = sizeof(NVS_STORAGE_EMPTY_JSON);
    char *json = malloc(json_len);
    if (json == NULL)
    {
        return ESP_ERR_NO_MEM;
    }

    memcpy(json, NVS_STORAGE_EMPTY_JSON, json_len);
    *out_json = json;
    return ESP_OK;
}

static esp_err_t nvs_storage_get_current_date_string(char *date_buffer, size_t buffer_size)
{
    pcf85263a_handle_t rtc_handle = pcf85263a_get_handle();
    pcf85263a_datetime_t datetime = {0};
    int written;

    if (date_buffer == NULL || buffer_size < NVS_STORAGE_DATE_STR_LEN)
    {
        return ESP_ERR_INVALID_ARG;
    }

    if (rtc_handle == NULL)
    {
        ESP_LOGE(TAG, "rtc handle unavailable");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t err = pcf85263a_get_datetime(rtc_handle, &datetime);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "read rtc datetime failed: %s", esp_err_to_name(err));
        return err;
    }

    written = snprintf(date_buffer,
                       buffer_size,
                       "%04u-%02u-%02u",
                       datetime.year,
                       datetime.month,
                       datetime.day);
    if (written < 0 || (size_t)written >= buffer_size)
    {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t nvs_storage_is_midnight(bool *is_midnight)
{
    pcf85263a_handle_t rtc_handle = pcf85263a_get_handle();
    pcf85263a_datetime_t datetime = {0};

    if (is_midnight == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    if (rtc_handle == NULL)
    {
        ESP_LOGE(TAG, "rtc handle unavailable");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t err = pcf85263a_get_datetime(rtc_handle, &datetime);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "read rtc datetime failed: %s", esp_err_to_name(err));
        return err;
    }

    *is_midnight = (datetime.hour == 0U) && (datetime.minute == 0U);
    return ESP_OK;
}

static esp_err_t nvs_storage_load_records_json_alloc(char **out_json)
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_STORAGE_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "open nvs namespace failed: %s", esp_err_to_name(err));
        return err;
    }

    size_t required_size = 0;
    err = nvs_get_str(nvs_handle, NVS_STORAGE_KEY_RECORDS, NULL, &required_size);
    if (err == ESP_ERR_NVS_NOT_FOUND)
    {
        nvs_close(nvs_handle);
        return nvs_storage_alloc_empty_json(out_json);
    }
    if (err != ESP_OK)
    {
        nvs_close(nvs_handle);
        ESP_LOGE(TAG, "read records json size failed: %s", esp_err_to_name(err));
        return err;
    }

    char *json = malloc(required_size);
    if (json == NULL)
    {
        nvs_close(nvs_handle);
        return ESP_ERR_NO_MEM;
    }

    err = nvs_get_str(nvs_handle, NVS_STORAGE_KEY_RECORDS, json, &required_size);
    nvs_close(nvs_handle);
    if (err != ESP_OK)
    {
        free(json);
        ESP_LOGE(TAG, "read records json failed: %s", esp_err_to_name(err));
        return err;
    }

    *out_json = json;
    return ESP_OK;
}

static esp_err_t nvs_storage_store_records_json(const char *json)
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_STORAGE_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "open nvs namespace for write failed: %s", esp_err_to_name(err));
        return err;
    }

    err = nvs_set_str(nvs_handle, NVS_STORAGE_KEY_RECORDS, json);
    if (err == ESP_OK)
    {
        err = nvs_commit(nvs_handle);
    }
    nvs_close(nvs_handle);

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "store records json failed: %s", esp_err_to_name(err));
    }
    return err;
}

static cJSON *nvs_storage_parse_or_create_root(const char *json)
{
    cJSON *root = cJSON_Parse(json);
    if (root != NULL && cJSON_IsObject(root))
    {
        return root;
    }

    if (root != NULL)
    {
        cJSON_Delete(root);
    }

    ESP_LOGW(TAG, "records json invalid, resetting to empty root");
    root = cJSON_CreateObject();
    if (root == NULL)
    {
        return NULL;
    }

    if (cJSON_AddArrayToObject(root, "records") == NULL)
    {
        cJSON_Delete(root);
        return NULL;
    }

    return root;
}

static cJSON *nvs_storage_get_or_create_records_array(cJSON *root)
{
    cJSON *records = cJSON_GetObjectItemCaseSensitive(root, "records");
    if (cJSON_IsArray(records))
    {
        return records;
    }

    if (records != NULL)
    {
        cJSON_DeleteItemFromObjectCaseSensitive(root, "records");
    }

    return cJSON_AddArrayToObject(root, "records");
}

static cJSON *nvs_storage_find_record_by_date(cJSON *records, const char *date_string)
{
    cJSON *record = NULL;

    cJSON_ArrayForEach(record, records)
    {
        cJSON *date_item = cJSON_GetObjectItemCaseSensitive(record, "date");
        if (cJSON_IsString(date_item) && date_item->valuestring != NULL &&
            strcmp(date_item->valuestring, date_string) == 0)
        {
            return record;
        }
    }

    return NULL;
}

static esp_err_t nvs_storage_upsert_string_field(cJSON *object, const char *name, const char *value)
{
    cJSON *field = cJSON_GetObjectItemCaseSensitive(object, name);
    cJSON *replacement = cJSON_CreateString(value);

    if (replacement == NULL)
    {
        return ESP_ERR_NO_MEM;
    }

    if (field != NULL)
    {
        cJSON_ReplaceItemInObjectCaseSensitive(object, name, replacement);
    }
    else
    {
        cJSON_AddItemToObject(object, name, replacement);
    }

    return ESP_OK;
}

static esp_err_t nvs_storage_upsert_number_field(cJSON *object, const char *name, uint32_t value)
{
    cJSON *field = cJSON_GetObjectItemCaseSensitive(object, name);
    cJSON *replacement = cJSON_CreateNumber((double)value);

    if (replacement == NULL)
    {
        return ESP_ERR_NO_MEM;
    }

    if (field != NULL)
    {
        cJSON_ReplaceItemInObjectCaseSensitive(object, name, replacement);
    }
    else
    {
        cJSON_AddItemToObject(object, name, replacement);
    }

    return ESP_OK;
}

static esp_err_t nvs_storage_set_record_fields(cJSON *record,
                                               const char *date_string,
                                               const nvs_storage_daily_totals_t *totals)
{
    esp_err_t err = nvs_storage_upsert_string_field(record, "date", date_string);
    if (err != ESP_OK)
    {
        return err;
    }

    err = nvs_storage_upsert_number_field(record, "focus_count", totals->focus_count);
    if (err != ESP_OK)
    {
        return err;
    }

    err = nvs_storage_upsert_number_field(record, "nap_count", totals->nap_count);
    if (err != ESP_OK)
    {
        return err;
    }

    err = nvs_storage_upsert_number_field(record, "focus_minutes", totals->focus_minutes);
    if (err != ESP_OK)
    {
        return err;
    }

    return nvs_storage_upsert_number_field(record, "rest_minutes", totals->rest_minutes);
}

static void nvs_storage_trim_records(cJSON *records)
{
    while (cJSON_GetArraySize(records) > NVS_STORAGE_MAX_RECORDS)
    {
        cJSON_DeleteItemFromArray(records, 0);
    }
}

static esp_err_t nvs_storage_merge_record(cJSON *root,
                                          const char *date_string,
                                          const nvs_storage_daily_totals_t *totals)
{
    cJSON *records = nvs_storage_get_or_create_records_array(root);
    if (records == NULL)
    {
        return ESP_ERR_NO_MEM;
    }

    cJSON *record = nvs_storage_find_record_by_date(records, date_string);
    if (record == NULL)
    {
        record = cJSON_CreateObject();
        if (record == NULL)
        {
            return ESP_ERR_NO_MEM;
        }
        cJSON_AddItemToArray(records, record);
    }

    esp_err_t err = nvs_storage_set_record_fields(record, date_string, totals);
    if (err != ESP_OK)
    {
        return err;
    }

    nvs_storage_trim_records(records);
    return ESP_OK;
}

static uint32_t nvs_storage_get_record_uint(const cJSON *record, const char *name)
{
    const cJSON *item = cJSON_GetObjectItemCaseSensitive(record, name);
    if (!cJSON_IsNumber(item))
    {
        return 0;
    }

    if (item->valuedouble < 0)
    {
        return 0;
    }

    return (uint32_t)item->valuedouble;
}

static void nvs_storage_load_totals_from_record(const cJSON *record,
                                                nvs_storage_daily_totals_t *totals)
{
    totals->focus_count = (uint8_t)nvs_storage_get_record_uint(record, "focus_count");
    totals->nap_count = (uint8_t)nvs_storage_get_record_uint(record, "nap_count");
    totals->focus_minutes = (uint16_t)nvs_storage_get_record_uint(record, "focus_minutes");
    totals->rest_minutes = (uint16_t)nvs_storage_get_record_uint(record, "rest_minutes");
}

static esp_err_t nvs_storage_load_today_totals(const char *date_string,
                                               nvs_storage_daily_totals_t *totals)
{
    char *json = NULL;
    cJSON *root = NULL;
    cJSON *records = NULL;
    cJSON *record = NULL;
    esp_err_t err = nvs_storage_load_records_json_alloc(&json);
    if (err != ESP_OK)
    {
        return err;
    }

    root = nvs_storage_parse_or_create_root(json);
    free(json);
    if (root == NULL)
    {
        return ESP_ERR_NO_MEM;
    }

    records = nvs_storage_get_or_create_records_array(root);
    if (records == NULL)
    {
        cJSON_Delete(root);
        return ESP_ERR_NO_MEM;
    }

    memset(totals, 0, sizeof(*totals));
    record = nvs_storage_find_record_by_date(records, date_string);
    if (record != NULL)
    {
        nvs_storage_load_totals_from_record(record, totals);
    }

    cJSON_Delete(root);
    return ESP_OK;
}

static esp_err_t nvs_storage_export_json_internal(char *buffer,
                                                  size_t buffer_size,
                                                  size_t *out_length,
                                                  bool include_in_memory_totals)
{
    nvs_storage_daily_totals_t totals_snapshot = {0};
    char date_snapshot[NVS_STORAGE_DATE_STR_LEN] = {0};
    char *json = NULL;
    char *rendered_json = NULL;
    cJSON *root = NULL;
    esp_err_t err;

    if (include_in_memory_totals)
    {
        err = nvs_storage_save_daily_record();
        if (err != ESP_OK)
        {
            return err;
        }
    }

    err = nvs_storage_load_records_json_alloc(&json);
    if (err != ESP_OK)
    {
        return err;
    }

    root = nvs_storage_parse_or_create_root(json);
    free(json);
    if (root == NULL)
    {
        return ESP_ERR_NO_MEM;
    }

    if (include_in_memory_totals)
    {
        portENTER_CRITICAL(&s_nvs_storage_lock);
        totals_snapshot = s_nvs_storage_state.totals;
        memcpy(date_snapshot, s_nvs_storage_state.current_date, sizeof(date_snapshot));
        portEXIT_CRITICAL(&s_nvs_storage_lock);

        if (date_snapshot[0] != '\0' && !nvs_storage_totals_is_empty(&totals_snapshot))
        {
            err = nvs_storage_merge_record(root, date_snapshot, &totals_snapshot);
            if (err != ESP_OK)
            {
                cJSON_Delete(root);
                return err;
            }
        }
    }

    rendered_json = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    if (rendered_json == NULL)
    {
        return ESP_ERR_NO_MEM;
    }

    size_t rendered_len = strlen(rendered_json);
    if (buffer == NULL || buffer_size == 0)
    {
        cJSON_free(rendered_json);
        return ESP_ERR_INVALID_ARG;
    }
    if (buffer_size <= rendered_len)
    {
        cJSON_free(rendered_json);
        return ESP_ERR_INVALID_SIZE;
    }

    memcpy(buffer, rendered_json, rendered_len + 1U);
    cJSON_free(rendered_json);
    if (out_length != NULL)
    {
        *out_length = rendered_len;
    }
    return ESP_OK;
}

static esp_err_t nvs_storage_write_record_for_date(const char *date_string,
                                                   const nvs_storage_daily_totals_t *totals)
{
    char *json = NULL;
    char *rendered_json = NULL;
    cJSON *root = NULL;
    esp_err_t err;

    if (date_string == NULL || date_string[0] == '\0' || totals == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    if (nvs_storage_totals_is_empty(totals))
    {
        return ESP_OK;
    }

    err = nvs_storage_load_records_json_alloc(&json);
    if (err != ESP_OK)
    {
        return err;
    }

    root = nvs_storage_parse_or_create_root(json);
    free(json);
    if (root == NULL)
    {
        return ESP_ERR_NO_MEM;
    }

    err = nvs_storage_merge_record(root, date_string, totals);
    if (err != ESP_OK)
    {
        cJSON_Delete(root);
        return err;
    }

    rendered_json = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    if (rendered_json == NULL)
    {
        return ESP_ERR_NO_MEM;
    }

    err = nvs_storage_store_records_json(rendered_json);
    cJSON_free(rendered_json);
    return err;
}

esp_err_t nvs_storage_init(void)
{
    char current_date[NVS_STORAGE_DATE_STR_LEN] = {0};
    nvs_storage_daily_totals_t persisted_totals = {0};
    esp_err_t err = nvs_storage_get_current_date_string(current_date, sizeof(current_date));
    if (err != ESP_OK)
    {
        return err;
    }

    err = nvs_storage_load_today_totals(current_date, &persisted_totals);
    if (err != ESP_OK)
    {
        return err;
    }

    portENTER_CRITICAL(&s_nvs_storage_lock);
    s_nvs_storage_state.initialized = true;
    memcpy(s_nvs_storage_state.current_date, current_date, sizeof(s_nvs_storage_state.current_date));
    s_nvs_storage_state.totals = persisted_totals;
    portEXIT_CRITICAL(&s_nvs_storage_lock);

    ESP_LOGI(TAG,
             "storage initialized for %s with focus=%u nap=%u focus_min=%u rest_min=%u",
             s_nvs_storage_state.current_date,
             s_nvs_storage_state.totals.focus_count,
             s_nvs_storage_state.totals.nap_count,
             s_nvs_storage_state.totals.focus_minutes,
             s_nvs_storage_state.totals.rest_minutes);
    return ESP_OK;
}

esp_err_t nvs_storage_save_daily_record(void)
{
    char tracked_date[NVS_STORAGE_DATE_STR_LEN] = {0};
    nvs_storage_daily_totals_t totals_snapshot = {0};

    if (!s_nvs_storage_state.initialized)
    {
        return ESP_ERR_INVALID_STATE;
    }

    portENTER_CRITICAL(&s_nvs_storage_lock);
    totals_snapshot = s_nvs_storage_state.totals;
    memcpy(tracked_date, s_nvs_storage_state.current_date, sizeof(tracked_date));
    portEXIT_CRITICAL(&s_nvs_storage_lock);

    if (tracked_date[0] == '\0')
    {
        return ESP_ERR_INVALID_STATE;
    }

    return nvs_storage_write_record_for_date(tracked_date, &totals_snapshot);
}

esp_err_t nvs_storage_sync_current_day(void)
{
    char rtc_date[NVS_STORAGE_DATE_STR_LEN] = {0};
    char tracked_date[NVS_STORAGE_DATE_STR_LEN] = {0};
    nvs_storage_daily_totals_t tracked_totals = {0};
    nvs_storage_daily_totals_t next_day_totals = {0};
    esp_err_t err;

    if (!s_nvs_storage_state.initialized)
    {
        return ESP_ERR_INVALID_STATE;
    }

    err = nvs_storage_get_current_date_string(rtc_date, sizeof(rtc_date));
    if (err != ESP_OK)
    {
        return err;
    }

    portENTER_CRITICAL(&s_nvs_storage_lock);
    memcpy(tracked_date, s_nvs_storage_state.current_date, sizeof(tracked_date));
    tracked_totals = s_nvs_storage_state.totals;
    portEXIT_CRITICAL(&s_nvs_storage_lock);

    if (tracked_date[0] == '\0')
    {
        err = nvs_storage_load_today_totals(rtc_date, &next_day_totals);
        if (err != ESP_OK)
        {
            return err;
        }

        portENTER_CRITICAL(&s_nvs_storage_lock);
        memcpy(s_nvs_storage_state.current_date, rtc_date, sizeof(s_nvs_storage_state.current_date));
        s_nvs_storage_state.totals = next_day_totals;
        portEXIT_CRITICAL(&s_nvs_storage_lock);
        return ESP_OK;
    }

    if (strcmp(tracked_date, rtc_date) == 0)
    {
        return ESP_OK;
    }

    err = nvs_storage_write_record_for_date(tracked_date, &tracked_totals);
    if (err != ESP_OK)
    {
        return err;
    }

    err = nvs_storage_load_today_totals(rtc_date, &next_day_totals);
    if (err != ESP_OK)
    {
        return err;
    }

    portENTER_CRITICAL(&s_nvs_storage_lock);
    memcpy(s_nvs_storage_state.current_date, rtc_date, sizeof(s_nvs_storage_state.current_date));
    s_nvs_storage_state.totals = next_day_totals;
    portEXIT_CRITICAL(&s_nvs_storage_lock);

    ESP_LOGI(TAG, "rolled daily totals from %s to %s", tracked_date, rtc_date);
    return ESP_OK;
}

esp_err_t nvs_storage_get_records_json(char *buffer, size_t buffer_size, size_t *out_length)
{
    if (!s_nvs_storage_state.initialized)
    {
        return ESP_ERR_INVALID_STATE;
    }

    return nvs_storage_export_json_internal(buffer, buffer_size, out_length, true);
}

esp_err_t nvs_storage_accumulate_focus_minutes(uint16_t minutes)
{
    portENTER_CRITICAL(&s_nvs_storage_lock);
    if (!s_nvs_storage_state.initialized)
    {
        portEXIT_CRITICAL(&s_nvs_storage_lock);
        return ESP_ERR_INVALID_STATE;
    }
    s_nvs_storage_state.totals.focus_minutes += minutes;
    portEXIT_CRITICAL(&s_nvs_storage_lock);
    return ESP_OK;
}

esp_err_t nvs_storage_accumulate_rest_minutes(uint16_t minutes)
{
    portENTER_CRITICAL(&s_nvs_storage_lock);
    if (!s_nvs_storage_state.initialized)
    {
        portEXIT_CRITICAL(&s_nvs_storage_lock);
        return ESP_ERR_INVALID_STATE;
    }
    s_nvs_storage_state.totals.rest_minutes += minutes;
    portEXIT_CRITICAL(&s_nvs_storage_lock);
    return ESP_OK;
}

esp_err_t nvs_storage_increment_focus_count(void)
{
    portENTER_CRITICAL(&s_nvs_storage_lock);
    if (!s_nvs_storage_state.initialized)
    {
        portEXIT_CRITICAL(&s_nvs_storage_lock);
        return ESP_ERR_INVALID_STATE;
    }
    s_nvs_storage_state.totals.focus_count++;
    portEXIT_CRITICAL(&s_nvs_storage_lock);
    return ESP_OK;
}

esp_err_t nvs_storage_increment_nap_count(void)
{
    portENTER_CRITICAL(&s_nvs_storage_lock);
    if (!s_nvs_storage_state.initialized)
    {
        portEXIT_CRITICAL(&s_nvs_storage_lock);
        return ESP_ERR_INVALID_STATE;
    }
    s_nvs_storage_state.totals.nap_count++;
    portEXIT_CRITICAL(&s_nvs_storage_lock);
    return ESP_OK;
}

esp_err_t nvs_storage_reset_daily_counters(void)
{
    char current_date[NVS_STORAGE_DATE_STR_LEN] = {0};
    bool has_current_date = nvs_storage_get_current_date_string(current_date, sizeof(current_date)) == ESP_OK;

    portENTER_CRITICAL(&s_nvs_storage_lock);
    if (!s_nvs_storage_state.initialized)
    {
        portEXIT_CRITICAL(&s_nvs_storage_lock);
        return ESP_ERR_INVALID_STATE;
    }
    memset(&s_nvs_storage_state.totals, 0, sizeof(s_nvs_storage_state.totals));
    if (has_current_date)
    {
        memcpy(s_nvs_storage_state.current_date, current_date, sizeof(s_nvs_storage_state.current_date));
    }
    portEXIT_CRITICAL(&s_nvs_storage_lock);
    return ESP_OK;
}

esp_err_t nvs_storage_get_daily_totals(nvs_storage_daily_totals_t *totals)
{
    if (totals == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    portENTER_CRITICAL(&s_nvs_storage_lock);
    if (!s_nvs_storage_state.initialized)
    {
        portEXIT_CRITICAL(&s_nvs_storage_lock);
        return ESP_ERR_INVALID_STATE;
    }
    *totals = s_nvs_storage_state.totals;
    portEXIT_CRITICAL(&s_nvs_storage_lock);
    return ESP_OK;
}

esp_err_t nvs_storage_get_tracked_date(char *date_buffer, size_t buffer_size)
{
    if (date_buffer == NULL || buffer_size < NVS_STORAGE_DATE_STR_LEN)
    {
        return ESP_ERR_INVALID_ARG;
    }

    portENTER_CRITICAL(&s_nvs_storage_lock);
    if (!s_nvs_storage_state.initialized)
    {
        portEXIT_CRITICAL(&s_nvs_storage_lock);
        return ESP_ERR_INVALID_STATE;
    }
    memcpy(date_buffer, s_nvs_storage_state.current_date, NVS_STORAGE_DATE_STR_LEN);
    portEXIT_CRITICAL(&s_nvs_storage_lock);
    return ESP_OK;
}

esp_err_t nvs_storage_set_daily_counts(uint8_t focus_count, uint8_t nap_count)
{
    portENTER_CRITICAL(&s_nvs_storage_lock);
    if (!s_nvs_storage_state.initialized)
    {
        portEXIT_CRITICAL(&s_nvs_storage_lock);
        return ESP_ERR_INVALID_STATE;
    }
    s_nvs_storage_state.totals.focus_count = focus_count;
    s_nvs_storage_state.totals.nap_count = nap_count;
    portEXIT_CRITICAL(&s_nvs_storage_lock);
    return ESP_OK;
}

esp_err_t nvs_storage_set_daily_minutes(uint16_t focus_minutes, uint16_t rest_minutes)
{
    portENTER_CRITICAL(&s_nvs_storage_lock);
    if (!s_nvs_storage_state.initialized)
    {
        portEXIT_CRITICAL(&s_nvs_storage_lock);
        return ESP_ERR_INVALID_STATE;
    }
    s_nvs_storage_state.totals.focus_minutes = focus_minutes;
    s_nvs_storage_state.totals.rest_minutes = rest_minutes;
    portEXIT_CRITICAL(&s_nvs_storage_lock);
    return ESP_OK;
}
