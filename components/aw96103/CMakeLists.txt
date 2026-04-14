idf_component_register(
    SRCS "aw96103.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_driver_i2c esp_timer esp_driver_gpio freertos pinmap 
)
