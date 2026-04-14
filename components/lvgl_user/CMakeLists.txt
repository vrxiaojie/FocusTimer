idf_component_register(SRCS "lvgl_user.c"
                    PRIV_REQUIRES st7305_2p9 esp_timer spi_shared_lock
                    INCLUDE_DIRS "include")
