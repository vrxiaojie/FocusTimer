#pragma once

#include <stdbool.h>

#include "esp_err.h"

void main_screen_start_update_task(void);
void main_screen_stop_update_task(void);
void main_screen_set_auto_wakeup_refreshing(bool refreshing);
void update_main_screen_date_labels(bool acquire_lock);
esp_err_t main_screen_start_sensor_refresh(void);
esp_err_t main_screen_refresh_once(uint32_t timeout_ms);
esp_err_t main_screen_refresh_once_pending(uint32_t timeout_ms);
void main_screen_start_idle_detect(void);
void main_screen_stop_idle_detect(void);
