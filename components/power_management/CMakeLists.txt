idf_component_register(SRCS "power_management.c"
                    REQUIRES esp_pm nvs_flash esp_timer driver pinmap esp_lcd st7305_2p9 pcf85263a
                    INCLUDE_DIRS "include")
