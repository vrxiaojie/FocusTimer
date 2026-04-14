#include "spi_shared_lock.h"

#include "freertos/semphr.h"

static SemaphoreHandle_t s_spi_shared_mutex = NULL;

void spi_shared_lock_init(void)
{
    if (s_spi_shared_mutex == NULL) {
        s_spi_shared_mutex = xSemaphoreCreateMutex();
    }
}

bool spi_shared_lock_take(TickType_t ticks_to_wait)
{
    if (s_spi_shared_mutex == NULL) {
        return true;
    }

    return xSemaphoreTake(s_spi_shared_mutex, ticks_to_wait) == pdTRUE;
}

void spi_shared_lock_give(void)
{
    if (s_spi_shared_mutex != NULL) {
        xSemaphoreGive(s_spi_shared_mutex);
    }
}
