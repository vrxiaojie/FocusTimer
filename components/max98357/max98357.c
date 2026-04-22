#include "max98357.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"

#include "driver/gpio.h"
#include "esp_log.h"
#include "pinmap.h"
#include "spi_shared_lock.h"

typedef struct {
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint16_t bits_per_sample;
} wav_format_t;

typedef struct {
    uint32_t data_offset;
    uint32_t data_size;
    wav_format_t fmt;
} wav_info_t;

typedef struct {
    char id[4];
    uint32_t size;
} __attribute__((packed)) wav_chunk_header_t;

static const char *TAG = "max98357";

static esp_err_t s_config_power_pin(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << AUDIO_SD,
        .mode = GPIO_MODE_OUTPUT_OD,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    return gpio_config(&io_conf);
}

static void s_cleanup_tx_channel(max98357_handle_t *handle)
{
    if (handle->tx_chan != NULL) {
        i2s_channel_disable(handle->tx_chan);
        i2s_del_channel(handle->tx_chan);
        handle->tx_chan = NULL;
    }
}

static esp_err_t s_parse_wav_header(FILE *f, wav_info_t *out_info)
{
    uint8_t riff_header[12];
    bool fmt_found = false;
    bool data_found = false;

    if (fread(riff_header, 1, sizeof(riff_header), f) != sizeof(riff_header)) {
        return ESP_FAIL;
    }

    if (memcmp(riff_header, "RIFF", 4) != 0 || memcmp(&riff_header[8], "WAVE", 4) != 0) {
        return ESP_ERR_INVALID_ARG;
    }

    while (!data_found) {
        wav_chunk_header_t chunk;
        size_t read_len = fread(&chunk, 1, sizeof(chunk), f);
        if (read_len == 0) {
            break;
        }
        if (read_len != sizeof(chunk)) {
            return ESP_FAIL;
        }

        if (memcmp(chunk.id, "fmt ", 4) == 0) {
            uint8_t fmt_data[16];
            if (chunk.size < sizeof(fmt_data)) {
                return ESP_ERR_INVALID_SIZE;
            }
            if (fread(fmt_data, 1, sizeof(fmt_data), f) != sizeof(fmt_data)) {
                return ESP_FAIL;
            }

            out_info->fmt.audio_format = (uint16_t)(fmt_data[0] | (fmt_data[1] << 8));
            out_info->fmt.num_channels = (uint16_t)(fmt_data[2] | (fmt_data[3] << 8));
            out_info->fmt.sample_rate = (uint32_t)(fmt_data[4] | (fmt_data[5] << 8) | (fmt_data[6] << 16) | (fmt_data[7] << 24));
            out_info->fmt.bits_per_sample = (uint16_t)(fmt_data[14] | (fmt_data[15] << 8));
            fmt_found = true;

            if (chunk.size > sizeof(fmt_data)) {
                if (fseek(f, (long)(chunk.size - sizeof(fmt_data)), SEEK_CUR) != 0) {
                    return ESP_FAIL;
                }
            }
        } else if (memcmp(chunk.id, "data", 4) == 0) {
            out_info->data_offset = (uint32_t)ftell(f);
            out_info->data_size = chunk.size;
            data_found = true;
        } else {
            if (fseek(f, (long)chunk.size, SEEK_CUR) != 0) {
                return ESP_FAIL;
            }
        }

        if (chunk.size & 0x1U) {
            if (fseek(f, 1, SEEK_CUR) != 0) {
                return ESP_FAIL;
            }
        }
    }

    if (!fmt_found || !data_found) {
        return ESP_ERR_NOT_FOUND;
    }

    if (out_info->fmt.audio_format != 1) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    if (out_info->fmt.bits_per_sample != 16) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    if (out_info->fmt.num_channels != 1 && out_info->fmt.num_channels != 2) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}

static void s_apply_volume_16bit(int16_t *samples, size_t sample_count, uint8_t volume_percent)
{
    for (size_t i = 0; i < sample_count; i++) {
        int32_t scaled = ((int32_t)samples[i] * volume_percent) / 100;
        if (scaled > 32767) {
            scaled = 32767;
        } else if (scaled < -32768) {
            scaled = -32768;
        }
        samples[i] = (int16_t)scaled;
    }
}

void max98357_get_default_config(max98357_config_t *out_config)
{
    if (out_config == NULL) {
        return;
    }

    *out_config = (max98357_config_t) {
        .i2s_port = I2S_NUM_0,
        .pin_bclk = 23,
        .pin_ws = 24,
        .pin_dout = 10,
        .sample_rate_hz = 22050,
        .volume_percent = 10,
        .stream_buffer_bytes = 2048,
    };
}

esp_err_t max98357_set_config(max98357_handle_t *handle, const max98357_config_t *config)
{
    if (handle == NULL || config == NULL || config->stream_buffer_bytes == 0 || config->volume_percent > 100) {
        return ESP_ERR_INVALID_ARG;
    }

    if (handle->tx_chan != NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    handle->config = *config;
    return ESP_OK;
}

esp_err_t max98357_init(max98357_handle_t *handle)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = s_config_power_pin();
    if (ret != ESP_OK) {
        return ret;
    }

    // 默认关闭功放，等到播放时才打开
    ret = gpio_set_level(AUDIO_SD, 0);
    if (ret != ESP_OK) {
        return ret;
    }

    handle->initialized = 1;
    return ESP_OK;
}

esp_err_t max98357_deinit(max98357_handle_t *handle)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    (void)max98357_set_enabled(handle, false);
    s_cleanup_tx_channel(handle);

    handle->initialized = 0;
    return ESP_OK;
}

esp_err_t max98357_set_enabled(max98357_handle_t *handle, bool enable)
{
    if (handle == NULL || !handle->initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    return gpio_set_level(AUDIO_SD, enable ? 1 : 0);
}

esp_err_t max98357_play_wav_file(max98357_handle_t *handle, const char *path)
{
    if (handle == NULL || path == NULL || !handle->initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t ret;
    wav_info_t wav = {0};

    if (!spi_shared_lock_take(portMAX_DELAY)) {
        return ESP_ERR_TIMEOUT;
    }

    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        spi_shared_lock_give();
        ESP_LOGE(TAG, "Failed to open WAV file: %s", path);
        return ESP_FAIL;
    }

    ret = s_parse_wav_header(f, &wav);
    if (ret != ESP_OK) {
        fclose(f);
        spi_shared_lock_give();
        return ret;
    }

    i2s_slot_mode_t slot_mode = (wav.fmt.num_channels == 2) ? I2S_SLOT_MODE_STEREO : I2S_SLOT_MODE_MONO;

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(handle->config.i2s_port, I2S_ROLE_MASTER);
    ret = i2s_new_channel(&chan_cfg, &handle->tx_chan, NULL);
    if (ret != ESP_OK) {
        fclose(f);
        spi_shared_lock_give();
        return ret;
    }

    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(wav.fmt.sample_rate),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, slot_mode),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = handle->config.pin_bclk,
            .ws = handle->config.pin_ws,
            .dout = handle->config.pin_dout,
            .din = I2S_GPIO_UNUSED,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };

    ret = i2s_channel_init_std_mode(handle->tx_chan, &std_cfg);
    if (ret != ESP_OK) {
        s_cleanup_tx_channel(handle);
        fclose(f);
        spi_shared_lock_give();
        return ret;
    }

    ret = i2s_channel_enable(handle->tx_chan);
    if (ret != ESP_OK) {
        s_cleanup_tx_channel(handle);
        fclose(f);
        spi_shared_lock_give();
        return ret;
    }

    if (fseek(f, (long)wav.data_offset, SEEK_SET) != 0) {
        fclose(f);
        s_cleanup_tx_channel(handle);
        spi_shared_lock_give();
        return ESP_FAIL;
    }

    spi_shared_lock_give();

    uint8_t *buffer = malloc(handle->config.stream_buffer_bytes);
    if (buffer == NULL) {
        fclose(f);
        s_cleanup_tx_channel(handle);
        return ESP_ERR_NO_MEM;
    }

    uint32_t remaining = wav.data_size;
    while (remaining > 0) {
        size_t req = remaining > handle->config.stream_buffer_bytes ? handle->config.stream_buffer_bytes : remaining;

        if (!spi_shared_lock_take(portMAX_DELAY)) {
            free(buffer);
            fclose(f);
            s_cleanup_tx_channel(handle);
            return ESP_ERR_TIMEOUT;
        }

        size_t bytes_read = fread(buffer, 1, req, f);
        spi_shared_lock_give();

        if (bytes_read == 0) {
            break;
        }

        if (bytes_read & 0x1U) {
            bytes_read--;
        }
        if (bytes_read == 0) {
            break;
        }

        s_apply_volume_16bit((int16_t *)buffer, bytes_read / sizeof(int16_t), handle->config.volume_percent);

        size_t bytes_written = 0;
        ret = i2s_channel_write(handle->tx_chan, buffer, bytes_read, &bytes_written, portMAX_DELAY);
        if (ret != ESP_OK) {
            free(buffer);
            fclose(f);
            s_cleanup_tx_channel(handle);
            return ret;
        }

        remaining -= (uint32_t)bytes_read;
    }

    free(buffer);

    if (!spi_shared_lock_take(portMAX_DELAY)) {
        s_cleanup_tx_channel(handle);
        return ESP_ERR_TIMEOUT;
    }

    fclose(f);
    spi_shared_lock_give();

    s_cleanup_tx_channel(handle);

    return ESP_OK;
}
