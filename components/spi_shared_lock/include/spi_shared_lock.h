#pragma once

#include <stdbool.h>

#include "freertos/FreeRTOS.h"

void spi_shared_lock_init(void);
bool spi_shared_lock_take(TickType_t ticks_to_wait);
void spi_shared_lock_give(void);
