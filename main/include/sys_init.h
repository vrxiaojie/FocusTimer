#pragma once

#include "esp_err.h"
#include "sdspi.h"
#include "max98357.h"
#include "esp_lcd_io_spi.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_io.h"

extern esp_lcd_panel_handle_t panel_handle;
extern esp_lcd_panel_io_handle_t io_handle;
extern sdspi_handle_t sd_handle;
extern max98357_handle_t audio_handle;

esp_err_t spi_bus_init();
esp_err_t sdcard_init(sdspi_handle_t *handle);
esp_err_t audio_init(max98357_handle_t *handle);
esp_err_t lcd_screen_init();