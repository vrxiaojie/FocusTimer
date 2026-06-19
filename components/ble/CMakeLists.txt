idf_component_register(
    SRCS "ble.c"
    INCLUDE_DIRS "include"
    REQUIRES bt nvs_flash json
    PRIV_REQUIRES nvs_storage pcf85263a power_management esp_pm
)
