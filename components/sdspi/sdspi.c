#include "sdspi.h"

#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "driver/gpio.h"
#include "driver/spi_common.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "spi_shared_lock.h"

static const char *TAG = "sdspi_comp";

static bool s_has_wav_extension(const char *name)
{
    size_t len = strlen(name);
    if (len < 4) {
        return false;
    }

    const char *ext = &name[len - 4];
    return (tolower((unsigned char)ext[0]) == '.' &&
            tolower((unsigned char)ext[1]) == 'w' &&
            tolower((unsigned char)ext[2]) == 'a' &&
            tolower((unsigned char)ext[3]) == 'v');
}

static int s_compare_paths(const void *a, const void *b)
{
    const char *pa = (const char *)a;
    const char *pb = (const char *)b;
    return strcmp(pa, pb);
}

static void s_enable_pullups(const sdspi_config_t *cfg)
{
    gpio_set_pull_mode(cfg->pin_miso, GPIO_PULLUP_ONLY);
}

void sdspi_get_default_config(sdspi_config_t *out_config)
{
    if (out_config == NULL) {
        return;
    }

    *out_config = (sdspi_config_t) {
        .mount_point = "/sdcard",
        .pin_miso = 0,
        .pin_mosi = 0,
        .pin_clk = 0,
        .pin_cs = 0,
        .max_freq_khz = 1000,
        .enable_internal_pullups = true,
        .format_if_mount_failed = false,
        .max_open_files = 5,
        .allocation_unit_size = 16 * 1024,
    };
}

esp_err_t sdspi_set_config(sdspi_handle_t *handle, const sdspi_config_t *config)
{
    if (handle == NULL || config == NULL || config->mount_point == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (handle->mounted) {
        return ESP_ERR_INVALID_STATE;
    }

    handle->config = *config;
    return ESP_OK;
}

// 注意：在调用该函数之前，先使用spi_bus_initialize初始化SPI总线
esp_err_t sdspi_init(sdspi_handle_t *handle)
{
    
    if (handle == NULL || handle->config.mount_point == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (handle->mounted) {
        return ESP_OK;
    }

    esp_err_t ret;
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.max_freq_khz = handle->config.max_freq_khz;

    if (handle->config.enable_internal_pullups) {
        s_enable_pullups(&handle->config);
    }

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = handle->config.pin_cs;
    slot_config.host_id = host.slot;

    esp_vfs_fat_sdmmc_mount_config_t mount_cfg = {
        .format_if_mount_failed = handle->config.format_if_mount_failed,
        .max_files = handle->config.max_open_files,
        .allocation_unit_size = handle->config.allocation_unit_size,
    };

    ret = esp_vfs_fat_sdspi_mount(handle->config.mount_point, &host, &slot_config, &mount_cfg, &handle->card);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_vfs_fat_sdspi_mount failed: %s", esp_err_to_name(ret));
        return ret;
    }

    handle->host_slot = host.slot;
    handle->mounted = true;
    return ESP_OK;
}

esp_err_t sdspi_deinit(sdspi_handle_t *handle)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!handle->mounted) {
        return ESP_OK;
    }

    esp_vfs_fat_sdcard_unmount(handle->config.mount_point, handle->card);

    handle->card = NULL;
    handle->mounted = false;
    return ESP_OK;
}

esp_err_t sdspi_collect_wav_files(const sdspi_handle_t *handle, sdspi_file_list_t *out_list)
{
    if (handle == NULL || out_list == NULL || !handle->mounted) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!spi_shared_lock_take(portMAX_DELAY)) {
        return ESP_ERR_TIMEOUT;
    }

    DIR *dir = opendir(handle->config.mount_point);
    if (dir == NULL) {
        spi_shared_lock_give();
        ESP_LOGE(TAG, "Failed to open dir: %s", handle->config.mount_point);
        return ESP_FAIL;
    }

    out_list->count = 0;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') {
            continue;
        }
        if (!s_has_wav_extension(entry->d_name)) {
            continue;
        }
        if (out_list->count >= SDSPI_MAX_WAV_FILES) {
            ESP_LOGW(TAG, "Too many wav files; max=%d", SDSPI_MAX_WAV_FILES);
            break;
        }

        int n = snprintf(out_list->paths[out_list->count], SDSPI_MAX_PATH_LEN, "%s/%s",
                         handle->config.mount_point, entry->d_name);
        if (n <= 0 || n >= SDSPI_MAX_PATH_LEN) {
            ESP_LOGW(TAG, "Skip long name: %s", entry->d_name);
            continue;
        }

        out_list->count++;
    }

    closedir(dir);
    spi_shared_lock_give();

    if (out_list->count == 0) {
        return ESP_ERR_NOT_FOUND;
    }

    qsort(out_list->paths, out_list->count, sizeof(out_list->paths[0]), s_compare_paths);
    return ESP_OK;
}
