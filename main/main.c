#include <stdio.h>
#include <sys/lock.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#include "esp_lcd_io_spi.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_io.h"

#include "lvgl.h"
#include "ui.h"
#include "screens.h"

#include "st7305_2p9.h"
#include "lvgl_user.h"
#include "pinmap.h"

#define TAG "main"

esp_lcd_panel_handle_t panel_handle = NULL;

void app_main(void)
{
    spi_bus_config_t buscfg = {
        .sclk_io_num = SPI_CLK_PIN,
        .mosi_io_num = SPI_MOSI_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4096,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO)); // 启用 DMA

    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = SCREEN_DC_PIN,
        .cs_gpio_num = SCREEN_CS_PIN,
        .pclk_hz = 40000000,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .spi_mode = 0,
        .trans_queue_depth = 10,
    };
    // 将 LCD 连接到 SPI 总线
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI2_HOST, &io_config, &io_handle));

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = -1, // 复位接在芯片EN引脚上
        .bits_per_pixel = 1,
    };
    esp_lcd_new_panel_st7305(io_handle, &panel_config, &panel_handle);
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
    // esp_lcd_panel_st7305_set_power_mode(panel_handle, ST7305_PWR_MODE_LPM);
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, false)); // 调整反色

    lvgl_user_init(panel_handle, io_handle);
    ui_init();
}