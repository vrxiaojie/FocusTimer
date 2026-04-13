#pragma once
#include "stdio.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_vendor.h"

#define LCD_H_RES 168
#define LCD_V_RES 384

typedef enum {
    ST7305_PWR_MODE_HPM = 0, // High Power Mode
    ST7305_PWR_MODE_LPM = 1, // Low Power Mode
} st7305_power_mode_t;

esp_err_t esp_lcd_new_panel_st7305(const esp_lcd_panel_io_handle_t io, const esp_lcd_panel_dev_config_t *panel_dev_config, esp_lcd_panel_handle_t *ret_panel);
esp_err_t esp_lcd_panel_st7305_set_power_mode(esp_lcd_panel_t *panel, st7305_power_mode_t power_mode);
st7305_power_mode_t esp_lcd_panel_st7305_get_power_mode(esp_lcd_panel_t *panel);
esp_err_t esp_lcd_panel_st7305_sleep(esp_lcd_panel_t *panel, bool sleep);
