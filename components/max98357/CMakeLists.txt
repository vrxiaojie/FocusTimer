idf_component_register(
    SRCS "max98357.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_driver_i2s spi_shared_lock
)
