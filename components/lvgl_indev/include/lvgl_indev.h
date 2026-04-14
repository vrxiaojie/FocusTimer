#pragma once

#include "lvgl.h"

void lvgl_indev_init();
void aw_touch_key_event_cb(uint8_t key_index, bool pressed, void *user_ctx);