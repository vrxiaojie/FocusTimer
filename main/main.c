#include <stdio.h>
#include <sys/lock.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_sleep.h"

#include "lvgl.h"
#include "ui.h"
#include "screens.h"

#include "spi_shared_lock.h"
#include "lvgl_user.h"
#include "sys_init.h"
#include "aw96103.h"
#include "lvgl_indev.h"
#include "stcc4.h"
#include "pcf85263a.h"
#include "imu.h"
#include "aw32001.h"
#include "battery.h"
#include "nvs_storage.h"
#include "power_management.h"
#include "st7305_2p9.h"
#include "sleep.h"
#include "pinmap.h"

#define TAG "main"

void app_main(void)
{
    esp_sleep_wakeup_cause_t wake_cause = esp_sleep_get_wakeup_cause();
    ESP_LOGI(TAG, "boot, wakeup cause=%d", (int)wake_cause);

    ESP_ERROR_CHECK_WITHOUT_ABORT(sleep_init_nvs_flash());
    bool wakeup_from_timer = power_management_is_wakeup_from_timer();
    bool wakeup_by_touch = power_management_is_wakeup_by_touch();

    if (wakeup_from_timer)
    {
        sleep_handle_timer_wakeup();
        return;
    }

    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_init());
    ESP_ERROR_CHECK_WITHOUT_ABORT(pcf85263a_init(I2C_NUM_0));
    ESP_ERROR_CHECK_WITHOUT_ABORT(nvs_storage_init());
    sleep_sync_daily_record_on_midnight_wakeup();
    ESP_ERROR_CHECK_WITHOUT_ABORT(aw96103_init());
    ESP_ERROR_CHECK_WITHOUT_ABORT(stcc4_i2c_init(I2C_NUM_0));
    ESP_ERROR_CHECK_WITHOUT_ABORT(imu_init(I2C_NUM_0));
    ESP_ERROR_CHECK_WITHOUT_ABORT(aw32001_init(I2C_NUM_0));
    aw96103_register_key_event_cb(aw_touch_key_event_cb, NULL);
    ESP_ERROR_CHECK_WITHOUT_ABORT(battery_init());
    sleep_register_pre_deepsleep_cb();
    spi_shared_lock_init();
    ESP_ERROR_CHECK_WITHOUT_ABORT(spi_bus_init());
    ESP_ERROR_CHECK_WITHOUT_ABORT(sdcard_init(&sd_handle));
    ESP_ERROR_CHECK_WITHOUT_ABORT(audio_init(&audio_handle));

    ESP_ERROR_CHECK_WITHOUT_ABORT(lcd_screen_init());
    lvgl_user_init(panel_handle, io_handle);
    power_management_register_panel(panel_handle);
    ESP_ERROR_CHECK_WITHOUT_ABORT(power_management_init());
    _lock_acquire(&lvgl_api_lock);
    create_screens();
    lv_scr_load(objects.main);
    lvgl_indev_init();
    _lock_release(&lvgl_api_lock);
    sleep_register_ble_datetime_cb();
    sleep_register_ble_power_settings_cb();
    
    /* 如果是触摸唤醒，通常意味着用户要操作。
       由于 ext1 是电平唤醒，触摸那一下不一定会再产生一次下降沿中断，
       因此这里主动恢复屏幕到 HPM，避免"已唤醒但界面不刷新/无法操作"的假死观感。 */
    if (wakeup_by_touch)
    {
        power_management_notify_user_activity();
    }

    if (!wakeup_from_timer && !wakeup_by_touch)
    {
        esp_lcd_panel_st7305_set_power_mode(panel_handle, ST7305_PWR_MODE_HPM);
        /* 正常启动：显示启动画面 */
        _lock_acquire(&lvgl_api_lock);
        lv_scr_load_anim(objects.start, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
        _lock_release(&lvgl_api_lock);
        ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
        vTaskDelay(pdTICKS_TO_MS(1000));
        _lock_acquire(&lvgl_api_lock);
        lv_scr_load_anim(objects.main, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 300, 0, true);
        _lock_release(&lvgl_api_lock);
    }
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
    imu_start_flip_detection_task();

    // 注册shipping mode回调并启动电源按键检测
    sleep_register_shipping_mode_cb();
    aw32001_power_key_init();
    vTaskDelay(pdMS_TO_TICKS(50));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
}