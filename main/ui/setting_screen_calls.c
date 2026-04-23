#include "setting_screen_calls.h"
#include "main_screen_calls.h"
#include <stdio.h>
#include <stdbool.h>
#include "lvgl.h"
#include "pcf85263a.h"
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "ui.h"

static const char *TAG = "setting_scr";

enum
{
    SETTING_FIELD_YEAR = 0,
    SETTING_FIELD_MONTH = 1,
    SETTING_FIELD_DAY = 2,
    SETTING_FIELD_WEEKDAY = 3,
    SETTING_FIELD_HOUR = 4,
    SETTING_FIELD_MINUTE = 5,
    SETTING_FIELD_COUNT,
};

static const char *const s_weekday_text[7] = {
    "星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"};

static int8_t s_setting_active_field = -1;
static bool s_setting_editing = false;

static lv_obj_t *setting_get_field_btn(uint8_t field)
{
    switch (field)
    {
    case SETTING_FIELD_YEAR:
        return objects.setting_scr_year_btn;
    case SETTING_FIELD_MONTH:
        return objects.setting_scr_month_btn;
    case SETTING_FIELD_DAY:
        return objects.setting_scr_day_btn;
    case SETTING_FIELD_WEEKDAY:
        return objects.setting_scr_weekday_btn;
    case SETTING_FIELD_HOUR:
        return objects.setting_scr_hour_btn;
    case SETTING_FIELD_MINUTE:
        return objects.setting_scr_minute_btn;
    default:
        return NULL;
    }
}

static lv_obj_t *setting_get_field_value_label(uint8_t field)
{
    lv_obj_t *btn = setting_get_field_btn(field);
    if (btn == NULL)
    {
        return NULL;
    }
    return lv_obj_get_child(btn, 0);
}

static int32_t setting_wrap(int32_t value, int32_t min, int32_t max)
{
    if (value < min)
    {
        return max;
    }
    if (value > max)
    {
        return min;
    }
    return value;
}

static int32_t setting_parse_weekday_text(const char *text)
{
    if (text == NULL)
    {
        return 0;
    }

    for (int32_t i = 0; i < 7; i++)
    {
        if (strcmp(text, s_weekday_text[i]) == 0)
        {
            return i;
        }
    }

    char *end_ptr = NULL;
    long val = strtol(text, &end_ptr, 10);
    if (end_ptr != text && val >= 0 && val <= 6)
    {
        return (int32_t)val;
    }

    return 0;
}

static int32_t setting_get_field_value(uint8_t field)
{
    lv_obj_t *label = setting_get_field_value_label(field);
    if (label == NULL)
    {
        return 0;
    }

    const char *text = lv_label_get_text(label);
    if (field == SETTING_FIELD_WEEKDAY)
    {
        return setting_parse_weekday_text(text);
    }

    if (text == NULL)
    {
        return 0;
    }

    char *end_ptr = NULL;
    long val = strtol(text, &end_ptr, 10);
    if (end_ptr == text)
    {
        return 0;
    }

    return (int32_t)val;
}

static void setting_set_field_value(uint8_t field, int32_t value)
{
    lv_obj_t *label = setting_get_field_value_label(field);
    if (label == NULL)
    {
        return;
    }

    switch (field)
    {
    case SETTING_FIELD_YEAR:
        if (value < 2000)
            value = 2000;
        if (value > 2099)
            value = 2099;
        lv_label_set_text_fmt(label, "%ld", (long)value);
        break;
    case SETTING_FIELD_MONTH:
        value = setting_wrap(value, 1, 12);
        lv_label_set_text_fmt(label, "%ld", (long)value);
        break;
    case SETTING_FIELD_DAY:
        value = setting_wrap(value, 1, 31);
        lv_label_set_text_fmt(label, "%ld", (long)value);
        break;
    case SETTING_FIELD_WEEKDAY:
        value = setting_wrap(value, 0, 6);
        lv_label_set_text(label, s_weekday_text[value]);
        break;
    case SETTING_FIELD_HOUR:
        value = setting_wrap(value, 0, 23);
        lv_label_set_text_fmt(label, "%02ld", (long)value);
        break;
    case SETTING_FIELD_MINUTE:
        value = setting_wrap(value, 0, 59);
        lv_label_set_text_fmt(label, "%02ld", (long)value);
        break;
    default:
        break;
    }
}

static void setting_set_editing_state(uint8_t field, bool editing)
{
    lv_obj_t *btn = setting_get_field_btn(field);
    if (btn == NULL)
    {
        return;
    }

    if (editing)
    {
        lv_obj_add_state(btn, LV_STATE_CHECKED);
    }
    else
    {
        lv_obj_remove_state(btn, LV_STATE_CHECKED);
    }
}

static bool setting_is_leap_year(uint16_t year)
{
    return ((year % 4U == 0U) && ((year % 100U != 0U) || (year % 400U == 0U)));
}

static int32_t setting_days_in_month(uint16_t year, uint8_t month)
{
    switch (month)
    {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        return 31;
    case 4:
    case 6:
    case 9:
    case 11:
        return 30;
    case 2:
        return setting_is_leap_year(year) ? 29 : 28;
    default:
        return 31;
    }
}

static void setting_adjust_day_for_month_year(void)
{
    int32_t year = setting_get_field_value(SETTING_FIELD_YEAR);
    int32_t month = setting_get_field_value(SETTING_FIELD_MONTH);
    int32_t day = setting_get_field_value(SETTING_FIELD_DAY);
    int32_t max_day = setting_days_in_month((uint16_t)year, (uint8_t)month);

    if (day < 1 || day > max_day)
    {
        setting_set_field_value(SETTING_FIELD_DAY, 1);
    }
}

void handle_setting_date_btn_event(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *target = (lv_obj_t *)lv_event_get_target(e);
    uint8_t field = (uint8_t)(uintptr_t)lv_event_get_user_data(e);
    lv_group_t *group = lv_obj_get_group(target);
    if (field >= SETTING_FIELD_COUNT)
    {
        return;
    }

    if (code == LV_EVENT_CLICKED)
    {
        bool same_field = (s_setting_active_field == (int8_t)field);
        bool was_editing = s_setting_editing;
        if (same_field)
        {
            s_setting_editing = !s_setting_editing;
        }
        else
        {
            if (s_setting_active_field >= 0)
            {
                setting_set_editing_state((uint8_t)s_setting_active_field, false);
            }
            s_setting_active_field = (int8_t)field;
            s_setting_editing = true;
        }

        if (group != NULL)
        {
            lv_group_set_editing(group, s_setting_editing);
        }

        setting_set_editing_state(field, s_setting_editing);
        if (!s_setting_editing)
        {
            if (was_editing && field == SETTING_FIELD_MONTH)
            {
                setting_adjust_day_for_month_year();
            }
            s_setting_active_field = -1;
        }
        return;
    }

    if (code == LV_EVENT_KEY)
    {
        if (!s_setting_editing || s_setting_active_field != (int8_t)field)
        {
            return;
        }

        uint32_t key = lv_event_get_key(e);
        int32_t delta = 0;
        if (key == LV_KEY_LEFT)
        {
            delta = -1;
        }
        else if (key == LV_KEY_RIGHT)
        {
            delta = 1;
        }
        else
        {
            return;
        }

        int32_t value = setting_get_field_value(field);
        if (field == SETTING_FIELD_DAY)
        {
            int32_t year = setting_get_field_value(SETTING_FIELD_YEAR);
            int32_t month = setting_get_field_value(SETTING_FIELD_MONTH);
            int32_t max_day = setting_days_in_month((uint16_t)year, (uint8_t)month);

            value += delta;
            if (value > max_day)
            {
                value = 1;
            }
            else if (value < 1)
            {
                value = max_day;
            }
            setting_set_field_value(field, value);
            return;
        }

        setting_set_field_value(field, value + delta);
        return;
    }

    if (code == LV_EVENT_DEFOCUSED)
    {
        setting_set_editing_state(field, false);
        if (s_setting_active_field == (int8_t)field)
        {
            s_setting_active_field = -1;
            s_setting_editing = false;
            if (group != NULL)
            {
                lv_group_set_editing(group, false);
            }
        }
    }
}

void handle_setting_screen_load_unload_event(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_SCREEN_LOAD_START)
    {
        pcf85263a_datetime_t dt = {0};
        pcf85263a_handle_t handle = pcf85263a_get_handle();

        if (handle == NULL)
        {
            ESP_LOGW(TAG, "RTC handle is NULL when loading setting screen");
            return;
        }

        esp_err_t err = pcf85263a_get_datetime(handle, &dt);
        if (err != ESP_OK)
        {
            ESP_LOGW(TAG, "Failed to read RTC datetime: %s", esp_err_to_name(err));
            return;
        }

        setting_set_field_value(SETTING_FIELD_YEAR, dt.year);
        setting_set_field_value(SETTING_FIELD_MONTH, dt.month);
        setting_set_field_value(SETTING_FIELD_DAY, dt.day);
        setting_set_field_value(SETTING_FIELD_WEEKDAY, dt.weekday);
        setting_set_field_value(SETTING_FIELD_HOUR, dt.hour);
        setting_set_field_value(SETTING_FIELD_MINUTE, dt.minute);

        s_setting_active_field = -1;
        s_setting_editing = false;
    }
    if (code == LV_EVENT_SCREEN_UNLOAD_START)
    {
        pcf85263a_datetime_t dt = {0};
        pcf85263a_handle_t handle = pcf85263a_get_handle();

        dt.year = (uint16_t)setting_get_field_value(SETTING_FIELD_YEAR);
        dt.month = (uint8_t)setting_get_field_value(SETTING_FIELD_MONTH);
        dt.day = (uint8_t)setting_get_field_value(SETTING_FIELD_DAY);
        dt.weekday = (uint8_t)setting_get_field_value(SETTING_FIELD_WEEKDAY);
        dt.hour = (uint8_t)setting_get_field_value(SETTING_FIELD_HOUR);
        dt.minute = (uint8_t)setting_get_field_value(SETTING_FIELD_MINUTE);
        dt.second = 0;

        if (handle == NULL)
        {
            ESP_LOGW(TAG, "RTC handle is NULL when unloading setting screen");
            return;
        }

        esp_err_t err = pcf85263a_set_datetime(handle, &dt);
        if (err != ESP_OK)
        {
            ESP_LOGW(TAG, "Failed to write RTC datetime: %s", esp_err_to_name(err));
        }
        update_main_screen_date_labels(false); // 设置完后立即在主屏幕上更新
    }
}