#include <stdio.h>
#include <sys/lock.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "driver/gpio.h"

#include "esp_check.h"
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
//test rtc interrupt -- start
#define RTC_INT_TASK_STACK_SIZE 2048
#define RTC_INT_TASK_PRIORITY 10
#define RTC_PERIODIC_INTERRUPT_MODE PCF85263A_PERIODIC_EVERY_MINUTE

static QueueHandle_t s_rtc_int_evt_queue = NULL;

static void IRAM_ATTR rtc_int_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)(uintptr_t)arg;
    BaseType_t high_task_woken = pdFALSE;

    if (s_rtc_int_evt_queue != NULL)
    {
        xQueueSendFromISR(s_rtc_int_evt_queue, &gpio_num, &high_task_woken);
    }

    if (high_task_woken == pdTRUE)
    {
        portYIELD_FROM_ISR();
    }
}

static void rtc_int_task(void *arg)
{
    (void)arg;

    uint32_t io_num = 0;

    while (1)
    {
        if (xQueueReceive(s_rtc_int_evt_queue, &io_num, portMAX_DELAY) != pdTRUE)
        {
            continue;
        }

        pcf85263a_handle_t rtc_handle = pcf85263a_get_handle();
        if (rtc_handle == NULL)
        {
            ESP_LOGW(TAG, "RTC INT GPIO%" PRIu32 ", but RTC handle is unavailable", io_num);
            continue;
        }

        uint8_t flags = 0;
        esp_err_t err = pcf85263a_get_flags(rtc_handle, &flags);
        if (err != ESP_OK)
        {
            ESP_LOGW(TAG, "read RTC flags failed after GPIO%" PRIu32 " interrupt: %s", io_num, esp_err_to_name(err));
            continue;
        }

        pcf85263a_datetime_t datetime = {0};
        err = pcf85263a_get_datetime(rtc_handle, &datetime);
        if (err == ESP_OK)
        {
            ESP_LOGI(TAG,
                     "RTC INT GPIO%" PRIu32 " falling edge, flags=0x%02x, time=%04u-%02u-%02u %02u:%02u:%02u",
                     io_num,
                     flags,
                     datetime.year,
                     datetime.month,
                     datetime.day,
                     datetime.hour,
                     datetime.minute,
                     datetime.second);
        }
        else
        {
            ESP_LOGW(TAG, "read RTC datetime failed after GPIO%" PRIu32 " interrupt: %s", io_num, esp_err_to_name(err));
        }

        if ((flags & PCF85263A_FLAG_PIF) != 0)
        {
            err = pcf85263a_clear_flags(rtc_handle, PCF85263A_FLAG_PIF);
            if (err != ESP_OK)
            {
                ESP_LOGW(TAG, "clear RTC periodic flag failed: %s", esp_err_to_name(err));
            }
        }
    }
}

static esp_err_t rtc_interrupt_init(void)
{
    pcf85263a_handle_t rtc_handle = pcf85263a_get_handle();
    if (rtc_handle == NULL)
    {
        return ESP_ERR_INVALID_STATE;
    }

    ESP_RETURN_ON_ERROR(pcf85263a_set_inta_mode(rtc_handle, PCF85263A_INTA_MODE_INTERRUPT),
                        TAG,
                        "set RTC INTA interrupt mode failed");
    ESP_RETURN_ON_ERROR(pcf85263a_set_inta_mask(rtc_handle, PCF85263A_INTA_ILP, true),
                        TAG,
                        "set RTC INTA level interrupt mode failed");
    ESP_RETURN_ON_ERROR(pcf85263a_enable_alarm1_interrupt(rtc_handle, false),
                        TAG,
                        "disable RTC alarm1 interrupt failed");
    ESP_RETURN_ON_ERROR(pcf85263a_clear_flags(rtc_handle, PCF85263A_FLAG_PIF | PCF85263A_FLAG_A1F),
                        TAG,
                        "clear RTC interrupt flags failed");
    ESP_RETURN_ON_ERROR(pcf85263a_set_periodic_interrupt(rtc_handle, RTC_PERIODIC_INTERRUPT_MODE),
                        TAG,
                        "set RTC periodic interrupt failed");

    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << RTC_INT_PIN,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };
    ESP_RETURN_ON_ERROR(gpio_config(&io_conf), TAG, "configure RTC INT GPIO failed");

    if (s_rtc_int_evt_queue == NULL)
    {
        s_rtc_int_evt_queue = xQueueCreate(10, sizeof(uint32_t));
        if (s_rtc_int_evt_queue == NULL)
        {
            return ESP_ERR_NO_MEM;
        }
    }

    esp_err_t err = gpio_install_isr_service(0);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE)
    {
        return err;
    }

    err = gpio_isr_handler_add(RTC_INT_PIN, rtc_int_isr_handler, (void *)(uintptr_t)RTC_INT_PIN);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE)
    {
        return err;
    }

    BaseType_t task_ok = xTaskCreate(rtc_int_task,
                                     "rtc_int_task",
                                     RTC_INT_TASK_STACK_SIZE,
                                     NULL,
                                     RTC_INT_TASK_PRIORITY,
                                     NULL);
    if (task_ok != pdPASS)
    {
        return ESP_ERR_NO_MEM;
    }

    if (gpio_get_level(RTC_INT_PIN) == 0)
    {
        uint32_t io_num = RTC_INT_PIN;
        (void)xQueueSend(s_rtc_int_evt_queue, &io_num, 0);
    }

    ESP_LOGI(TAG, "RTC periodic interrupt initialized on GPIO%d", RTC_INT_PIN);
    return ESP_OK;
}
//test rtc interrupt -- end

void app_main(void)
{
    esp_sleep_wakeup_cause_t wake_cause = esp_sleep_get_wakeup_cause();
    ESP_LOGI(TAG, "boot, wakeup cause=%d", (int)wake_cause);

    ESP_ERROR_CHECK_WITHOUT_ABORT(sleep_init_nvs_flash());
    bool wakeup_from_timer = power_management_is_wakeup_from_timer();
    bool wakeup_by_touch = power_management_is_wakeup_by_touch();
    bool wakeup_by_rtc = power_management_is_wakeup_by_rtc();

    if (wakeup_from_timer)
    {
        sleep_handle_rtc_wakeup();
        return;
    }

    if (wakeup_by_rtc && !wakeup_by_touch)
    {
        sleep_handle_rtc_wakeup();
        return;
    }

    ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_init());
    ESP_ERROR_CHECK_WITHOUT_ABORT(pcf85263a_init(I2C_NUM_0));
    ESP_ERROR_CHECK_WITHOUT_ABORT(rtc_interrupt_init());
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
