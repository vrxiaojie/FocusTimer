#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "pcf85263a.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief 初始化 NVS Flash（首次启动时擦除重建）
 */
esp_err_t sleep_init_nvs_flash(void);

/**
 * @brief 在午夜唤醒时同步历史记录
 */
void sleep_sync_daily_record_on_midnight_wakeup(void);

/**
 * @brief 处理 RTC 定时中断唤醒路径（简短亮屏后继续 deep sleep）
 */
void sleep_handle_rtc_wakeup(void);

/**
 * @brief 注册 deep sleep 前回调（关 BLE、IMU、屏幕降功耗等）
 */
void sleep_register_pre_deepsleep_cb(void);

/**
 * @brief 注册 shipping mode 回调
 */
void sleep_register_shipping_mode_cb(void);

/**
 * @brief 注册 BLE 时间更新回调
 */
void sleep_register_ble_datetime_cb(void);

/**
 * @brief 注册 BLE 电源设置更新回调
 */
void sleep_register_ble_power_settings_cb(void);

#ifdef __cplusplus
}
#endif
