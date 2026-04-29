#include "sys_init.h"
#include "st7305_2p9.h"
#include "lvgl_user.h"
#include "pinmap.h"
#include "sdspi.h"
#include "max98357.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "esp_lcd_io_spi.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_io.h"

#define TAG "sys_init"

esp_lcd_panel_handle_t panel_handle = NULL;
esp_lcd_panel_io_handle_t io_handle = NULL;
sdspi_handle_t sd_handle = {0};
max98357_handle_t audio_handle = {0};

esp_err_t spi_bus_init()
{
    esp_err_t ret = ESP_OK;
    // 将 LCD 的 CS 引脚设置为输出，并保持拉高，以避免在 SD 卡上电和初始化过程中 LCD 误选中
    gpio_config_t cs_cfg = {
        .pin_bit_mask = 1ULL << SCREEN_CS_PIN,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&cs_cfg));
    ESP_ERROR_CHECK(gpio_set_level(SCREEN_CS_PIN, 1));

    spi_bus_config_t buscfg = {
        .sclk_io_num = SPI_CLK_PIN,
        .mosi_io_num = SPI_MOSI_PIN,
        .miso_io_num = SPI_MISO_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = (LCD_H_RES * LCD_V_RES / 8) + 8,
    };
    ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO); // 启用 DMA
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "spi_bus_initialize failed: %s", esp_err_to_name(ret));
        return ret;
    }
    return ret;
}

esp_err_t sdcard_init(sdspi_handle_t *handle)
{
    esp_err_t ret = ESP_OK;
    sdspi_config_t sd_cfg;
    sdspi_get_default_config(&sd_cfg);
    sd_cfg.pin_miso = SPI_MISO_PIN;
    sd_cfg.pin_mosi = SPI_MOSI_PIN;
    sd_cfg.pin_clk = SPI_CLK_PIN;
    sd_cfg.pin_cs = SDCARD_CS_PIN;
    sd_cfg.max_freq_khz = 10000;           // 10MHz
    sd_cfg.enable_internal_pullups = true; // 启用MISO

    ret = sdspi_set_config(handle, &sd_cfg);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "sdspi_set_config failed: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = sdspi_init(handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "sdspi_init failed: %s", esp_err_to_name(ret));
        return ret;
    }
    return ESP_OK;
}

esp_err_t audio_init(max98357_handle_t *handle)
{
    esp_err_t ret = ESP_OK;
    max98357_config_t audio_cfg;
    max98357_get_default_config(&audio_cfg);
    audio_cfg.pin_bclk = I2S_BCLK_PIN;
    audio_cfg.pin_ws = I2S_LRC_PIN;
    audio_cfg.pin_dout = I2S_DOUT_PIN;
    audio_cfg.sample_rate_hz = 22050;
    audio_cfg.volume_percent = 10;
    audio_cfg.stream_buffer_bytes = 2048;

    ret = max98357_set_config(handle, &audio_cfg);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "max98357_set_config failed: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = max98357_init(handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "max98357_init failed: %s", esp_err_to_name(ret));
        return ret;
    }
    return ret;
}

esp_err_t lcd_screen_init()
{
    esp_err_t ret = ESP_OK;
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = SCREEN_DC_PIN,
        .cs_gpio_num = SCREEN_CS_PIN,
        .pclk_hz = 40000000,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .spi_mode = 0,
        .trans_queue_depth = 1,
    };
    // 将 LCD 连接到 SPI 总线
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI2_HOST, &io_config, &io_handle));

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = -1, // 复位接在芯片EN引脚上
        .bits_per_pixel = 1,
    };
    ret = esp_lcd_new_panel_st7305(io_handle, &panel_config, &panel_handle);
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, false));
    // esp_lcd_panel_st7305_set_power_mode(panel_handle, ST7305_PWR_MODE_LPM);
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, true)); // 调整反色
    return ret;
}

esp_err_t i2c_init()
{
    esp_err_t ret = ESP_OK;
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .scl_io_num = I2C_SCL_PIN,
        .sda_io_num = I2C_SDA_PIN,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = false,
    };

    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    return ret;
}