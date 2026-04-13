#pragma once

#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"
#include "driver/i2s_std.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int i2s_port;
    int pin_bclk;
    int pin_ws;
    int pin_dout;
    uint32_t sample_rate_hz;
    uint8_t volume_percent;
    size_t stream_buffer_bytes;
} max98357_config_t;

typedef struct {
    max98357_config_t config;
    i2s_chan_handle_t tx_chan;
    int initialized;
} max98357_handle_t;

void max98357_get_default_config(max98357_config_t *out_config);

esp_err_t max98357_set_config(max98357_handle_t *handle, const max98357_config_t *config);

esp_err_t max98357_init(max98357_handle_t *handle);

esp_err_t max98357_deinit(max98357_handle_t *handle);

esp_err_t max98357_play_wav_file(max98357_handle_t *handle, const char *path);

#ifdef __cplusplus
}
#endif
