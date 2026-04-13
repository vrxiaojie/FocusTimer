#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "esp_err.h"
#include "sdmmc_cmd.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SDSPI_MAX_WAV_FILES 32
#define SDSPI_MAX_PATH_LEN 128

typedef struct {
    const char *mount_point;
    int pin_miso;
    int pin_mosi;
    int pin_clk;
    int pin_cs;
    int max_freq_khz;
    bool enable_internal_pullups;
    bool format_if_mount_failed;
    size_t max_open_files;
    size_t allocation_unit_size;
} sdspi_config_t;

typedef struct {
    char paths[SDSPI_MAX_WAV_FILES][SDSPI_MAX_PATH_LEN];
    size_t count;
} sdspi_file_list_t;

typedef struct {
    sdspi_config_t config;
    sdmmc_card_t *card;
    int host_slot;
    bool mounted;
} sdspi_handle_t;

void sdspi_get_default_config(sdspi_config_t *out_config);

esp_err_t sdspi_set_config(sdspi_handle_t *handle, const sdspi_config_t *config);

esp_err_t sdspi_init(sdspi_handle_t *handle);

esp_err_t sdspi_deinit(sdspi_handle_t *handle);

esp_err_t sdspi_collect_wav_files(const sdspi_handle_t *handle, sdspi_file_list_t *out_list);

#ifdef __cplusplus
}
#endif
