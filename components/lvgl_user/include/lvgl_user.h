#pragma once

#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_io.h"
#include "lvgl.h"

extern _lock_t lvgl_api_lock;
extern lv_display_t *lvgl_display;
extern volatile lv_disp_rotation_t display_rotation;

void lvgl_user_init(esp_lcd_panel_handle_t panel_handle, esp_lcd_panel_io_handle_t io_handle);