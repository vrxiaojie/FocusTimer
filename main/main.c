#include <stdio.h>
#include <sys/lock.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

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
#include "message_screen_calls.h"

#define TAG "main"

static void shipping_mode_cb(void *user_data)
{
    _lock_acquire(&lvgl_api_lock);
    message_screen_show_with_text("", "关机中...\n如果正在充电,拔下电源后才会关机", "OK");
    _lock_release(&lvgl_api_lock);
}

void app_main(void)
{
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_init());
    ESP_ERROR_CHECK_WITHOUT_ABORT(pcf85263a_init(I2C_NUM_0));
    ESP_ERROR_CHECK_WITHOUT_ABORT(aw96103_init());
    ESP_ERROR_CHECK_WITHOUT_ABORT(stcc4_i2c_init(I2C_NUM_0));
    ESP_ERROR_CHECK_WITHOUT_ABORT(imu_init(I2C_NUM_0));
    ESP_ERROR_CHECK_WITHOUT_ABORT(aw32001_init(I2C_NUM_0));
    aw96103_register_key_event_cb(aw_touch_key_event_cb, NULL);
    ESP_ERROR_CHECK_WITHOUT_ABORT(battery_init());
    spi_shared_lock_init();
    ESP_ERROR_CHECK_WITHOUT_ABORT(spi_bus_init());
    ESP_ERROR_CHECK_WITHOUT_ABORT(sdcard_init(&sd_handle));
    ESP_ERROR_CHECK_WITHOUT_ABORT(audio_init(&audio_handle));

    // 屏幕及LVGL相关
    ESP_ERROR_CHECK_WITHOUT_ABORT(lcd_screen_init());
    lvgl_user_init(panel_handle, io_handle);
    _lock_acquire(&lvgl_api_lock);
    create_screens();
    lv_scr_load(objects.main);
    lv_scr_load_anim(objects.start, LV_SCR_LOAD_ANIM_NONE, 0, 0, false);
    lvgl_indev_init();
    _lock_release(&lvgl_api_lock);
    imu_start_flip_detection_task();

    // 注册shipping mode回调并启动电源按键检测
    aw32001_register_shipping_mode_cb(shipping_mode_cb, NULL);
    aw32001_power_key_init();
    vTaskDelay(pdMS_TO_TICKS(50));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
    vTaskDelay(pdTICKS_TO_MS(1000));
    _lock_acquire(&lvgl_api_lock);
    lv_scr_load_anim(objects.main, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 300, 0, true);
    _lock_release(&lvgl_api_lock);
}