#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"

struct esp_lcd_panel_t;
typedef struct esp_lcd_panel_t *esp_lcd_panel_handle_t;

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief 初始化电源管理模块
 *
 * - 从 NVS 读取持久化配置（低功耗模式、自动休眠、充电阈值）
 * - 若 NVS 无记录则使用默认值：低功耗=false, 自动休眠=false, 充电阈值=90
 * - 若低功耗模式已启用，则立即调用 esp_pm_configure
 * - 启动全局屏幕空闲检测，自动在无按键时切换屏幕到 LPM
 */
esp_err_t power_management_init(void);

/* ---- 低功耗模式（auto light sleep）---- */

esp_err_t power_management_set_auto_lightsleep(bool enable);
bool power_management_get_auto_lightsleep(void);

/**
 * @brief 注册用于屏幕功耗切换的 LCD panel 句柄
 */
void power_management_register_panel(esp_lcd_panel_handle_t panel);

/**
 * @brief 记录一次用户活动；必要时会先将屏幕切回高功耗模式
 */
void power_management_notify_user_activity(void);

/* ---- 自动休眠 ---- */

esp_err_t power_management_set_auto_sleep(bool enable);
bool power_management_get_auto_sleep(void);

/* ---- 充电阈值 ---- */

esp_err_t power_management_set_charge_threshold(uint8_t threshold);
uint8_t power_management_get_charge_threshold(void);

/* ---- 空闲计时器 ---- */

/**
 * @brief 重置空闲计时器（用户有操作时调用）
 */
void power_management_reset_idle_timer(void);

/**
 * @brief 启动全局空闲检测定时器（开机调用）
 */
void power_management_start_idle_timer(void);

/**
 * @brief 停止全局空闲检测定时器
 */
void power_management_stop_idle_timer(void);

/**
 * @brief 启动仅 main 屏幕生效的 deep sleep 空闲检测
 */
void power_management_start_deepsleep_idle_detect(void);

/**
 * @brief 停止仅 main 屏幕生效的 deep sleep 空闲检测
 */
void power_management_stop_deepsleep_idle_detect(void);

/* ---- Deep sleep hooks ---- */

typedef void (*power_management_hook_cb_t)(void *user_data);

/**
 * @brief 注册 deep sleep 前回调
 *
 * 在 power_management_enter_deepsleep() 内、调用 esp_deep_sleep_start() 前触发。
 * 典型用途：外设（如 IMU）在 deep sleep 前关断以降低静态功耗。
 */
esp_err_t power_management_register_pre_deepsleep_cb(power_management_hook_cb_t cb, void *user_data);

/* ---- Deep sleep / Wakeup ---- */

/**
 * @brief 是否从 deep sleep 唤醒（timer 或 ext1 wakeup）
 */
bool power_management_is_wakeup_from_sleep(void);

/**
 * @brief 是否因触摸操作从 deep sleep 唤醒
 */
bool power_management_is_wakeup_by_touch(void);
bool power_management_is_wakeup_by_rtc(void);
bool power_management_is_wakeup_from_timer(void);

/**
 * @brief 主动进入 deep sleep（RTC 每分钟中断唤醒 + 触摸中断唤醒）
 */
void power_management_enter_deepsleep(uint32_t wakeup_time_ms);

/**
 * @brief 主动进入 deep sleep（RTC Alarm1 在指定时间唤醒 + 触摸中断唤醒）
 */
void power_management_enter_deepsleep_until_rtc_time(uint8_t hour, uint8_t minute);

#ifdef __cplusplus
}
#endif
