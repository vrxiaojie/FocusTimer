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

#define TAG "main"

void app_main(void)
{
    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_init());
    ESP_ERROR_CHECK_WITHOUT_ABORT(aw96103_init());
    ESP_ERROR_CHECK_WITHOUT_ABORT(stcc4_i2c_init(I2C_NUM_0));
    aw96103_register_key_event_cb(aw_touch_key_event_cb, NULL);
    spi_shared_lock_init();
    ESP_ERROR_CHECK_WITHOUT_ABORT(spi_bus_init());
    ESP_ERROR_CHECK_WITHOUT_ABORT(sdcard_init(&sd_handle));
    ESP_ERROR_CHECK_WITHOUT_ABORT(audio_init(&audio_handle));
    ESP_ERROR_CHECK_WITHOUT_ABORT(lcd_screen_init());

    lvgl_user_init(panel_handle, io_handle);

    _lock_acquire(&lvgl_api_lock);
    ui_init();
    lvgl_indev_init();
    _lock_release(&lvgl_api_lock);
}