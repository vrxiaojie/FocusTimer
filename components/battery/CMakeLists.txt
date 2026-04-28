idf_component_register(SRCS "battery.c"
                    REQUIRES aw32001 esp_adc
                    PRIV_REQUIRES espressif__adc_battery_estimation
                    INCLUDE_DIRS "include")
