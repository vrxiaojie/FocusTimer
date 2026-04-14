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

#define TAG "main"

void app_main(void)
{
    spi_shared_lock_init();
    ESP_ERROR_CHECK_WITHOUT_ABORT(spi_bus_init());
    ESP_ERROR_CHECK_WITHOUT_ABORT(sdcard_init(&sd_handle));
    ESP_ERROR_CHECK_WITHOUT_ABORT(audio_init(&audio_handle));
    ESP_ERROR_CHECK_WITHOUT_ABORT(lcd_screen_init());

    lvgl_user_init(panel_handle, io_handle);
    ui_init();
}