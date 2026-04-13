/*
 * SPDX-FileCopyrightText: 2024
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <sys/cdefs.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_commands.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_check.h"
#include "st7305_2p9.h"

static const char *TAG = "st7305";

// ST7305 Commands
#define LCD_ST7305_CMD_SWRESET 0x01
#define LCD_ST7305_CMD_SLPIN 0x10
#define LCD_ST7305_CMD_SLPOUT 0x11
#define LCD_ST7305_CMD_INVOFF 0x20
#define LCD_ST7305_CMD_INVON 0x21
#define LCD_ST7305_CMD_DISPOFF 0x28
#define LCD_ST7305_CMD_DISPON 0x29
#define LCD_ST7305_CMD_CASET 0x2A
#define LCD_ST7305_CMD_RASET 0x2B
#define LCD_ST7305_CMD_RAMWR 0x2C
#define LCD_ST7305_CMD_MADCTL 0x36
#define LCD_ST7305_CMD_HPM 0x38
#define LCD_ST7305_CMD_LPM 0x39
#define LCD_ST7305_CMD_DTFORM 0x3A

typedef struct
{
    esp_lcd_panel_t base;
    esp_lcd_panel_io_handle_t io;
    int reset_gpio_num;
    bool reset_level;
    int x_gap;
    int y_gap;
    unsigned int bits_per_pixel;
    bool swap_axes;
    bool mirror_x;
    bool mirror_y;
    st7305_power_mode_t power_mode;
} st7305_panel_t;

static esp_err_t panel_st7305_del(esp_lcd_panel_t *panel);
static esp_err_t panel_st7305_reset(esp_lcd_panel_t *panel);
static esp_err_t panel_st7305_init(esp_lcd_panel_t *panel);
static esp_err_t panel_st7305_draw_bitmap(esp_lcd_panel_t *panel, int x_start, int y_start, int x_end, int y_end, const void *color_data);
static esp_err_t panel_st7305_invert_color(esp_lcd_panel_t *panel, bool invert_color_data);
static esp_err_t panel_st7305_mirror(esp_lcd_panel_t *panel, bool mirror_x, bool mirror_y);
static esp_err_t panel_st7305_swap_xy(esp_lcd_panel_t *panel, bool swap_axes);
static esp_err_t panel_st7305_set_gap(esp_lcd_panel_t *panel, int x_gap, int y_gap);
static esp_err_t panel_st7305_disp_on_off(esp_lcd_panel_t *panel, bool off);

esp_err_t esp_lcd_new_panel_st7305(const esp_lcd_panel_io_handle_t io, const esp_lcd_panel_dev_config_t *panel_dev_config, esp_lcd_panel_handle_t *ret_panel)
{
    esp_err_t ret = ESP_OK;
    st7305_panel_t *st7305 = NULL;
    ESP_GOTO_ON_FALSE(io && panel_dev_config && ret_panel, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    ESP_GOTO_ON_FALSE(panel_dev_config->bits_per_pixel == 1, ESP_ERR_INVALID_ARG, err, TAG, "bpp must be 1 for ST7305");

    st7305 = calloc(1, sizeof(st7305_panel_t));
    ESP_GOTO_ON_FALSE(st7305, ESP_ERR_NO_MEM, err, TAG, "no mem for st7305 panel");

    if (panel_dev_config->reset_gpio_num >= 0)
    {
        gpio_config_t io_conf = {
            .mode = GPIO_MODE_OUTPUT,
            .pin_bit_mask = 1ULL << panel_dev_config->reset_gpio_num,
        };
        ESP_GOTO_ON_ERROR(gpio_config(&io_conf), err, TAG, "configure GPIO for RST line failed");
    }

    st7305->io = io;
    st7305->bits_per_pixel = panel_dev_config->bits_per_pixel;
    st7305->reset_gpio_num = panel_dev_config->reset_gpio_num;
    st7305->reset_level = panel_dev_config->flags.reset_active_high;
    st7305->base.del = panel_st7305_del;
    st7305->base.reset = panel_st7305_reset;
    st7305->base.init = panel_st7305_init;
    st7305->base.draw_bitmap = panel_st7305_draw_bitmap;
    st7305->base.invert_color = panel_st7305_invert_color;
    st7305->base.set_gap = panel_st7305_set_gap;
    st7305->base.mirror = panel_st7305_mirror;
    st7305->base.swap_xy = panel_st7305_swap_xy;
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0)
    st7305->base.disp_off = panel_st7305_disp_on_off;
#else
    st7305->base.disp_on_off = panel_st7305_disp_on_off;
#endif
    *ret_panel = &(st7305->base);
    ESP_LOGD(TAG, "new st7305 panel @%p", st7305);

    return ESP_OK;

err:
    if (st7305)
    {
        if (panel_dev_config->reset_gpio_num >= 0)
        {
            gpio_reset_pin(panel_dev_config->reset_gpio_num);
        }
        free(st7305);
    }
    return ret;
}

static esp_err_t panel_st7305_del(esp_lcd_panel_t *panel)
{
    st7305_panel_t *st7305 = __containerof(panel, st7305_panel_t, base);
    if (st7305->reset_gpio_num >= 0)
    {
        gpio_reset_pin(st7305->reset_gpio_num);
    }
    ESP_LOGD(TAG, "del st7305 panel @%p", st7305);
    free(st7305);
    return ESP_OK;
}

static esp_err_t panel_st7305_reset(esp_lcd_panel_t *panel)
{
    st7305_panel_t *st7305 = __containerof(panel, st7305_panel_t, base);

    // perform hardware reset
    if (st7305->reset_gpio_num >= 0)
    {
        gpio_set_level(st7305->reset_gpio_num, st7305->reset_level);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_set_level(st7305->reset_gpio_num, !st7305->reset_level);
        vTaskDelay(pdMS_TO_TICKS(120));
    }
    else
    {
        // software reset
        esp_lcd_panel_io_tx_param(st7305->io, LCD_ST7305_CMD_SWRESET, NULL, 0);
        vTaskDelay(pdMS_TO_TICKS(120));
    }

    return ESP_OK;
}

static esp_err_t panel_st7305_init(esp_lcd_panel_t *panel)
{
    st7305_panel_t *st7305 = __containerof(panel, st7305_panel_t, base);
    esp_lcd_panel_io_handle_t io = st7305->io;

    // NVM Load
    esp_lcd_panel_io_tx_param(io, 0xD6, (uint8_t[]){0x13, 0x02}, 2);
    // Booster Enable
    esp_lcd_panel_io_tx_param(io, 0xD1, (uint8_t[]){0x01}, 1);

    // Gate Voltage Setting
    // VGH 00:8V  04:10V  08:12V   0E:15V   12:17V
    // VGL 00:-5V   04:-7V   0A:-10V
    esp_lcd_panel_io_tx_param(io, 0xC0, (uint8_t[]){0x12, 0x0A}, 2);

    // VLC=3.6V (12/-5)(delta Vp=0.6V)
    // VSHP Setting (4.8V) VSHP1 VSHP2 VSHP3 VSHP4
    esp_lcd_panel_io_tx_param(io, 0xC1, (uint8_t[]){0x3C, 0x3E, 0x3C, 0x3C}, 4);
    // VSLP Setting (0.98V) VSLP1 VSLP2 VSLP3 VSLP4
    esp_lcd_panel_io_tx_param(io, 0xC2, (uint8_t[]){0x23, 0x21, 0x23, 0x23}, 4);
    // VSHN Setting (-3.6V) VSHN1 VSHN2 VSHN3 VSHN4
    esp_lcd_panel_io_tx_param(io, 0xC4, (uint8_t[]){0x5A, 0x5C, 0x5A, 0x5A}, 4);
    // VSLN Setting (0.22V) VSLN1 VSLN2 VSLN3 VSLN4
    esp_lcd_panel_io_tx_param(io, 0xC5, (uint8_t[]){0x37, 0x35, 0x37, 0x37}, 4);

    // OSC Setting
    // Enable OSC, HPM Frame Rate Max = 32hZ
    esp_lcd_panel_io_tx_param(io, 0xD8, (uint8_t[]){0xA6, 0xE9}, 2);

    // Frame Rate Control
    /*-- HPM=32hz ; LPM=> 0x15=8Hz 0x14=4Hz 0x13=2Hz 0x12=1Hz 0x11=0.5Hz 0x10=0.25Hz---*/
    esp_lcd_panel_io_tx_param(io, 0xB2, (uint8_t[]){0x12}, 1); // HPM=32hz ; LPM=1hz

    // Update Period Gate EQ Control in HPM
    esp_lcd_panel_io_tx_param(io, 0xB3, (uint8_t[]){0xE5, 0xF6, 0x17, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x71}, 10);
    // Update Period Gate EQ Control in LPM
    esp_lcd_panel_io_tx_param(io, 0xB4, (uint8_t[]){0x05, 0x46, 0x77, 0x77, 0x77, 0x77, 0x76, 0x45}, 8);

    // Gate Timing Control
    esp_lcd_panel_io_tx_param(io, 0x62, (uint8_t[]){0x32, 0x03, 0x1F}, 3);

    // Source EQ Enable
    esp_lcd_panel_io_tx_param(io, 0xB7, (uint8_t[]){0x13}, 1);

    // Gate Line Setting
    // 384 line = 96 * 4
    esp_lcd_panel_io_tx_param(io, 0xB0, (uint8_t[]){0x60}, 1);

    // Sleep out
    esp_lcd_panel_io_tx_param(io, LCD_ST7305_CMD_SLPOUT, NULL, 0);
    vTaskDelay(pdMS_TO_TICKS(100));

    // Source Voltage Select
    // VSHP1; VSLP1 ; VSHN1 ; VSLN1
    esp_lcd_panel_io_tx_param(io, 0xC9, (uint8_t[]){0x00}, 1);

    // Memory Data Access Control
    esp_lcd_panel_io_tx_param(io, LCD_ST7305_CMD_MADCTL, (uint8_t[]){0x48}, 1); // Memory Data Access Control: MX=1 ; DO=1

    // Data Format Select
    esp_lcd_panel_io_tx_param(io, LCD_ST7305_CMD_DTFORM, (uint8_t[]){0x11}, 1); // 10:4write for 24bit ; 11: 3write for 24bit

    // Gamma Mode Setting
    esp_lcd_panel_io_tx_param(io, 0xB9, (uint8_t[]){0x20}, 1); // 20: Mono 00:4GS

    // Panel Setting
    esp_lcd_panel_io_tx_param(io, 0xB8, (uint8_t[]){0x29}, 1); // Panel Setting: 0x29: 1-Dot inversion, Frame inversion, One Line Interlace

    // WRITE RAM 168*384 Column Address Setting
    esp_lcd_panel_io_tx_param(io, LCD_ST7305_CMD_CASET, (uint8_t[]){0x17, 0x24}, 2); // 0X24-0X17=14 // 14*12=168

    // Row Address Setting
    esp_lcd_panel_io_tx_param(io, LCD_ST7305_CMD_RASET, (uint8_t[]){0x00, 0xBF}, 2); // 192*2=384

    // TE
    esp_lcd_panel_io_tx_param(io, 0x35, (uint8_t[]){0x00}, 1);

    // Auto power down ON
    esp_lcd_panel_io_tx_param(io, 0xD0, (uint8_t[]){0xFF}, 1);

    // HPM:high Power Mode ON
    esp_lcd_panel_io_tx_param(io, LCD_ST7305_CMD_HPM, NULL, 0);

    // DISPLAY ON
    esp_lcd_panel_io_tx_param(io, LCD_ST7305_CMD_DISPON, NULL, 0);

    vTaskDelay(pdMS_TO_TICKS(100));

    return ESP_OK;
}

static esp_err_t panel_st7305_draw_bitmap(esp_lcd_panel_t *panel, int x_start, int y_start, int x_end, int y_end, const void *color_data)
{
    st7305_panel_t *st7305 = __containerof(panel, st7305_panel_t, base);
    assert((x_start < x_end) && (y_start < y_end) && "start position must be smaller than end position");
    esp_lcd_panel_io_handle_t io = st7305->io;

    x_start += st7305->x_gap;
    x_end += st7305->x_gap;
    y_start += st7305->y_gap;
    y_end += st7305->y_gap;

    if (st7305->swap_axes)
    {
        int x = x_start;
        x_start = y_start;
        y_start = x;
        x = x_end;
        x_end = y_end;
        y_end = x;
    }

    // 根据 ST7305 特性及我们初始化的 0x3A 寄存器格式：
    // CASET 每个地址包含 3 个字节 (24bit)，对应 12 个像素宽。起始地址为 0x17 (23)
    // RASET 每个地址包含 2 个像素高，对应 1 行写入。起始地址为 0x00
    // 因此这里应当要求 x_start 能被 12 整除，y_start 能被 2 整除。
    int col_start = (x_start / 12) + 0x17;
    int col_end = ((x_end - 1) / 12) + 0x17;
    int row_start = y_start / 2;
    int row_end = (y_end - 1) / 2;

    esp_lcd_panel_io_tx_param(io, LCD_ST7305_CMD_CASET, (uint8_t[]){col_start, col_end}, 2);
    esp_lcd_panel_io_tx_param(io, LCD_ST7305_CMD_RASET, (uint8_t[]){row_start, row_end}, 2);

    // 计算实际需要发送的字节数
    // 每个 CASET 地址跨度需要发送 3 个 byte，每个 RASET 地址跨度就是纵向的一步
    size_t len = (col_end - col_start + 1) * 3 * (row_end - row_start + 1);
    esp_lcd_panel_io_tx_color(io, LCD_ST7305_CMD_RAMWR, color_data, len);

    return ESP_OK;
}

static esp_err_t panel_st7305_invert_color(esp_lcd_panel_t *panel, bool invert_color_data)
{
    st7305_panel_t *st7305 = __containerof(panel, st7305_panel_t, base);
    esp_lcd_panel_io_handle_t io = st7305->io;
    int command = invert_color_data ? LCD_ST7305_CMD_INVON : LCD_ST7305_CMD_INVOFF;
    esp_lcd_panel_io_tx_param(io, command, NULL, 0);
    return ESP_OK;
}

static esp_err_t panel_st7305_mirror(esp_lcd_panel_t *panel, bool mirror_x, bool mirror_y)
{
    st7305_panel_t *st7305 = __containerof(panel, st7305_panel_t, base);
    esp_lcd_panel_io_handle_t io = st7305->io;
    st7305->mirror_x = mirror_x;
    st7305->mirror_y = mirror_y;

    uint8_t madctl_val = 0;
    // According to MADCTL in ST7305
    // D7: MY, D6: MX, D5: MV
    if (st7305->mirror_y)
    {
        madctl_val |= 0x80;
    }
    if (st7305->mirror_x)
    {
        madctl_val |= 0x40;
    }
    if (st7305->swap_axes)
    {
        madctl_val |= 0x20;
    }

    esp_lcd_panel_io_tx_param(io, LCD_ST7305_CMD_MADCTL, &madctl_val, 1);
    return ESP_OK;
}

static esp_err_t panel_st7305_swap_xy(esp_lcd_panel_t *panel, bool swap_axes)
{
    st7305_panel_t *st7305 = __containerof(panel, st7305_panel_t, base);
    st7305->swap_axes = swap_axes;
    return panel_st7305_mirror(panel, st7305->mirror_x, st7305->mirror_y);
}

static esp_err_t panel_st7305_set_gap(esp_lcd_panel_t *panel, int x_gap, int y_gap)
{
    st7305_panel_t *st7305 = __containerof(panel, st7305_panel_t, base);
    st7305->x_gap = x_gap;
    st7305->y_gap = y_gap;
    return ESP_OK;
}

static esp_err_t panel_st7305_disp_on_off(esp_lcd_panel_t *panel, bool on_off)
{
    st7305_panel_t *st7305 = __containerof(panel, st7305_panel_t, base);
    esp_lcd_panel_io_handle_t io = st7305->io;

#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0)
    on_off = !on_off;
#endif

    int command = on_off ? LCD_ST7305_CMD_DISPON : LCD_ST7305_CMD_DISPOFF;
    esp_lcd_panel_io_tx_param(io, command, NULL, 0);
    return ESP_OK;
}

// 设置电源模式（高功耗/低功耗）
esp_err_t esp_lcd_panel_st7305_set_power_mode(esp_lcd_panel_t *panel, st7305_power_mode_t power_mode)
{
    st7305_panel_t *st7305 = __containerof(panel, st7305_panel_t, base);
    esp_lcd_panel_io_handle_t io = st7305->io;
    esp_lcd_panel_io_tx_param(io, power_mode == ST7305_PWR_MODE_HPM ? LCD_ST7305_CMD_HPM : LCD_ST7305_CMD_LPM, NULL, 0);
    st7305->power_mode = power_mode;
    return ESP_OK;
}

st7305_power_mode_t esp_lcd_panel_st7305_get_power_mode(esp_lcd_panel_t *panel)
{
    st7305_panel_t *st7305 = __containerof(panel, st7305_panel_t, base);
    return st7305->power_mode;
}

// 进入或退出睡眠模式
esp_err_t esp_lcd_panel_st7305_sleep(esp_lcd_panel_t *panel, bool sleep)
{
    st7305_panel_t *st7305 = __containerof(panel, st7305_panel_t, base);
    esp_lcd_panel_io_handle_t io = st7305->io;
    int command = sleep ? LCD_ST7305_CMD_SLPIN : LCD_ST7305_CMD_SLPOUT;
    esp_lcd_panel_io_tx_param(io, command, NULL, 0);
    return ESP_OK;
}