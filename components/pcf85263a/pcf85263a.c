#include <stdlib.h>
#include <string.h>

#include "esp_check.h"

#include "pcf85263a.h"

#define PCF85263A_REG_DT_100THS       (0x00)
#define PCF85263A_REG_DT_SECS         (0x01)
#define PCF85263A_REG_DT_MINUTES      (0x02)
#define PCF85263A_REG_DT_HOURS        (0x03)
#define PCF85263A_REG_DT_DAYS         (0x04)
#define PCF85263A_REG_DT_WEEKDAYS     (0x05)
#define PCF85263A_REG_DT_MONTHS       (0x06)
#define PCF85263A_REG_DT_YEARS        (0x07)

#define PCF85263A_REG_DT_SECOND_ALM1  (0x08)
#define PCF85263A_REG_DT_MINUTE_ALM1  (0x09)
#define PCF85263A_REG_DT_HOUR_ALM1    (0x0A)
#define PCF85263A_REG_DT_DAY_ALM1     (0x0B)
#define PCF85263A_REG_DT_MONTH_ALM1   (0x0C)
#define PCF85263A_REG_DT_ALARM_EN     (0x10)

#define PCF85263A_REG_CTRL_PIN_IO     (0x27)
#define PCF85263A_REG_CTRL_INTA_EN    (0x29)
#define PCF85263A_REG_CTRL_FLAGS      (0x2B)

#define PCF85263A_ALRM_SEC_A1E        (1U << 0)
#define PCF85263A_ALRM_MIN_A1E        (1U << 1)
#define PCF85263A_ALRM_HR_A1E         (1U << 2)
#define PCF85263A_ALRM_DAY_A1E        (1U << 3)
#define PCF85263A_ALRM_MON_A1E        (1U << 4)

#define PCF85263A_INT_A1IE            (1U << 4)
#define PCF85263A_PIN_IO_INTAPM_MASK  (0x03)

#define PCF85263A_SEC_MASK            (0x7F)
#define PCF85263A_MIN_MASK            (0x7F)
#define PCF85263A_HOUR_MASK           (0x3F)
#define PCF85263A_DAY_MASK            (0x3F)
#define PCF85263A_WEEKDAY_MASK        (0x07)
#define PCF85263A_MONTH_MASK          (0x1F)

static i2c_master_dev_handle_t pcf85263a_i2c_dev_handle = NULL;

static uint8_t dec_to_bcd(uint8_t val)
{
    return (uint8_t)(((val / 10U) << 4U) | (val % 10U));
}

static uint8_t bcd_to_dec(uint8_t val)
{
    return (uint8_t)(((val >> 4U) * 10U) + (val & 0x0FU));
}

static bool datetime_valid(const pcf85263a_datetime_t *dt)
{
    if ((dt->year < 2000U) || (dt->year > 2099U)) {
        return false;
    }
    if ((dt->month < 1U) || (dt->month > 12U)) {
        return false;
    }
    if ((dt->day < 1U) || (dt->day > 31U)) {
        return false;
    }
    if (dt->weekday > 6U) {
        return false;
    }
    if (dt->hour > 23U) {
        return false;
    }
    if (dt->minute > 59U) {
        return false;
    }
    if (dt->second > 59U) {
        return false;
    }
    return true;
}

static esp_err_t reg_read(pcf85263a_handle_t handle, uint8_t reg, uint8_t *data, size_t len)
{
    return i2c_master_transmit_receive(pcf85263a_i2c_dev_handle, &reg, 1, data, len, -1);
}

static esp_err_t reg_write(pcf85263a_handle_t handle, uint8_t reg, const uint8_t *data, size_t len)
{
    uint8_t payload[1 + 8];

    if (len > 8U) {
        return ESP_ERR_INVALID_SIZE;
    }

    payload[0] = reg;
    if (len > 0U) {
        memcpy(&payload[1], data, len);
    }

    return i2c_master_transmit(pcf85263a_i2c_dev_handle, payload, len + 1U, -1);
}

static esp_err_t reg_update_bits(pcf85263a_handle_t handle, uint8_t reg, uint8_t mask, uint8_t value)
{
    uint8_t current = 0;
    esp_err_t ret = reg_read(handle, reg, &current, 1);
    if (ret != ESP_OK) {
        return ret;
    }

    uint8_t next = (uint8_t)((current & (~mask)) | (value & mask));
    return reg_write(handle, reg, &next, 1);
}

esp_err_t pcf85263a_init(i2c_port_num_t port_num)
{
    i2c_master_bus_handle_t bus_handle;
    esp_err_t ret = ESP_OK;
    ret = i2c_master_get_bus_handle(port_num, &bus_handle);
    if (ret != ESP_OK) {
        return ret;
    }

    i2c_device_config_t i2c_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = PCF85263A_I2C_ADDRESS_DEFAULT,
        .scl_speed_hz = 400000,
    };

    ret = i2c_master_bus_add_device(bus_handle, &i2c_cfg, &pcf85263a_i2c_dev_handle);
    if (ret != ESP_OK) {
        return ret;
    }
    return ret;
}

esp_err_t pcf85263a_deinit()
{
    ESP_RETURN_ON_FALSE(pcf85263a_i2c_dev_handle != NULL, ESP_ERR_INVALID_ARG, "pcf85263a", "null handle");

    esp_err_t ret = i2c_master_bus_rm_device(pcf85263a_i2c_dev_handle);
    pcf85263a_i2c_dev_handle = NULL;
    return ret;
}

esp_err_t pcf85263a_set_datetime(pcf85263a_handle_t handle, const pcf85263a_datetime_t *dt)
{
    ESP_RETURN_ON_FALSE((handle != NULL) && (dt != NULL), ESP_ERR_INVALID_ARG, "pcf85263a", "null arg");
    ESP_RETURN_ON_FALSE(datetime_valid(dt), ESP_ERR_INVALID_ARG, "pcf85263a", "invalid datetime");

    uint8_t payload[8] = {
        0x00,
        dec_to_bcd(dt->second),
        dec_to_bcd(dt->minute),
        dec_to_bcd(dt->hour),
        dec_to_bcd(dt->day),
        (uint8_t)(dt->weekday & PCF85263A_WEEKDAY_MASK),
        dec_to_bcd(dt->month),
        dec_to_bcd((uint8_t)(dt->year - 2000U)),
    };

    return reg_write(handle, PCF85263A_REG_DT_100THS, payload, sizeof(payload));
}

esp_err_t pcf85263a_get_datetime(pcf85263a_handle_t handle, pcf85263a_datetime_t *dt)
{
    ESP_RETURN_ON_FALSE((handle != NULL) && (dt != NULL), ESP_ERR_INVALID_ARG, "pcf85263a", "null arg");

    uint8_t data[8] = {0};
    esp_err_t ret = reg_read(handle, PCF85263A_REG_DT_100THS, data, sizeof(data));
    if (ret != ESP_OK) {
        return ret;
    }

    dt->second = bcd_to_dec((uint8_t)(data[1] & PCF85263A_SEC_MASK));
    dt->minute = bcd_to_dec((uint8_t)(data[2] & PCF85263A_MIN_MASK));
    dt->hour = bcd_to_dec((uint8_t)(data[3] & PCF85263A_HOUR_MASK));
    dt->day = bcd_to_dec((uint8_t)(data[4] & PCF85263A_DAY_MASK));
    dt->weekday = (uint8_t)(data[5] & PCF85263A_WEEKDAY_MASK);
    dt->month = bcd_to_dec((uint8_t)(data[6] & PCF85263A_MONTH_MASK));
    dt->year = (uint16_t)(2000U + bcd_to_dec(data[7]));

    return ESP_OK;
}

esp_err_t pcf85263a_set_alarm1(pcf85263a_handle_t handle, const pcf85263a_alarm1_t *alarm)
{
    ESP_RETURN_ON_FALSE((handle != NULL) && (alarm != NULL), ESP_ERR_INVALID_ARG, "pcf85263a", "null arg");
    ESP_RETURN_ON_FALSE(alarm->second <= 59U, ESP_ERR_INVALID_ARG, "pcf85263a", "invalid alarm second");
    ESP_RETURN_ON_FALSE(alarm->minute <= 59U, ESP_ERR_INVALID_ARG, "pcf85263a", "invalid alarm minute");
    ESP_RETURN_ON_FALSE(alarm->hour <= 23U, ESP_ERR_INVALID_ARG, "pcf85263a", "invalid alarm hour");
    ESP_RETURN_ON_FALSE((alarm->day >= 1U) && (alarm->day <= 31U), ESP_ERR_INVALID_ARG, "pcf85263a", "invalid alarm day");
    ESP_RETURN_ON_FALSE((alarm->month >= 1U) && (alarm->month <= 12U), ESP_ERR_INVALID_ARG, "pcf85263a", "invalid alarm month");

    uint8_t alarm_data[5] = {
        dec_to_bcd(alarm->second),
        dec_to_bcd(alarm->minute),
        dec_to_bcd(alarm->hour),
        dec_to_bcd(alarm->day),
        dec_to_bcd(alarm->month),
    };

    esp_err_t ret = reg_write(handle, PCF85263A_REG_DT_SECOND_ALM1, alarm_data, sizeof(alarm_data));
    if (ret != ESP_OK) {
        return ret;
    }

    uint8_t alarm_en = 0;
    if (alarm->match_second) {
        alarm_en |= PCF85263A_ALRM_SEC_A1E;
    }
    if (alarm->match_minute) {
        alarm_en |= PCF85263A_ALRM_MIN_A1E;
    }
    if (alarm->match_hour) {
        alarm_en |= PCF85263A_ALRM_HR_A1E;
    }
    if (alarm->match_day) {
        alarm_en |= PCF85263A_ALRM_DAY_A1E;
    }
    if (alarm->match_month) {
        alarm_en |= PCF85263A_ALRM_MON_A1E;
    }

    return reg_update_bits(handle,
                           PCF85263A_REG_DT_ALARM_EN,
                           (PCF85263A_ALRM_SEC_A1E | PCF85263A_ALRM_MIN_A1E | PCF85263A_ALRM_HR_A1E |
                            PCF85263A_ALRM_DAY_A1E | PCF85263A_ALRM_MON_A1E),
                           alarm_en);
}

esp_err_t pcf85263a_enable_alarm1_interrupt(pcf85263a_handle_t handle, bool enable)
{
    ESP_RETURN_ON_FALSE(handle != NULL, ESP_ERR_INVALID_ARG, "pcf85263a", "null handle");

    return reg_update_bits(handle,
                           PCF85263A_REG_CTRL_INTA_EN,
                           PCF85263A_INT_A1IE,
                           enable ? PCF85263A_INT_A1IE : 0);
}

esp_err_t pcf85263a_set_inta_mask(pcf85263a_handle_t handle, uint8_t mask, bool enable)
{
    ESP_RETURN_ON_FALSE(handle != NULL, ESP_ERR_INVALID_ARG, "pcf85263a", "null handle");

    return reg_update_bits(handle,
                           PCF85263A_REG_CTRL_INTA_EN,
                           mask,
                           enable ? mask : 0);
}

esp_err_t pcf85263a_get_inta_mask(pcf85263a_handle_t handle, uint8_t *mask)
{
    ESP_RETURN_ON_FALSE((handle != NULL) && (mask != NULL), ESP_ERR_INVALID_ARG, "pcf85263a", "null arg");

    return reg_read(handle, PCF85263A_REG_CTRL_INTA_EN, mask, 1);
}

esp_err_t pcf85263a_get_flags(pcf85263a_handle_t handle, uint8_t *flags)
{
    ESP_RETURN_ON_FALSE((handle != NULL) && (flags != NULL), ESP_ERR_INVALID_ARG, "pcf85263a", "null arg");

    return reg_read(handle, PCF85263A_REG_CTRL_FLAGS, flags, 1);
}

esp_err_t pcf85263a_clear_flags(pcf85263a_handle_t handle, uint8_t flag_mask)
{
    ESP_RETURN_ON_FALSE(handle != NULL, ESP_ERR_INVALID_ARG, "pcf85263a", "null handle");

    return reg_update_bits(handle, PCF85263A_REG_CTRL_FLAGS, flag_mask, 0);
}

esp_err_t pcf85263a_set_inta_mode(pcf85263a_handle_t handle, pcf85263a_inta_mode_t mode)
{
    ESP_RETURN_ON_FALSE(handle != NULL, ESP_ERR_INVALID_ARG, "pcf85263a", "null handle");
    ESP_RETURN_ON_FALSE((uint8_t)mode <= 3U, ESP_ERR_INVALID_ARG, "pcf85263a", "invalid mode");

    return reg_update_bits(handle,
                           PCF85263A_REG_CTRL_PIN_IO,
                           PCF85263A_PIN_IO_INTAPM_MASK,
                           (uint8_t)mode);
}
