idf_component_register(SRCS "main.c" 
                    PRIV_REQUIRES esp_driver_spi esp_driver_touch_sens esp_pm esp_driver_i2s esp_timer nvs_flash esp_driver_gpio
                    REQUIRES st7305_2p9 lvgl_user pinmap sdspi max98357 spi_shared_lock aw96103 lvgl_indev stcc4
                    pcf85263a imu aw32001 battery nvs_storage power_management ble sleep ui sys_init
                    INCLUDE_DIRS "." )
