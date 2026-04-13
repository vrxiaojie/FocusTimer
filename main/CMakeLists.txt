set(UI_DIR ./ui)
file(GLOB_RECURSE UI_SRCS ${UI_DIR}/*.c ${UI_DIR}/*.cpp) 


idf_component_register(SRCS "main.c" ${UI_SRCS}
                    PRIV_REQUIRES esp_driver_spi esp_driver_touch_sens esp_pm esp_driver_i2s esp_timer
                    st7305_2p9 lvgl_user pinmap
                    INCLUDE_DIRS "." ${UI_DIR})
