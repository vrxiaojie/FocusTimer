idf_component_register(
    SRCS "max98357.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_driver_i2s esp_driver_gpio spi_shared_lock pinmap esp_partition
)
