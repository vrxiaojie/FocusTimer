#pragma once

#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C"
{
#endif

    esp_err_t battery_init(void);
    float battery_get_capacity(void);
    bool battery_is_charging(void);

#ifdef __cplusplus
}
#endif
