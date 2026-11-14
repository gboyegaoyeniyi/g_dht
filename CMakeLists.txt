idf_component_register(SRCS "g_dht.c"
                       INCLUDE_DIRS "include"
                       REQUIRES esp_driver_gpio
                       PRIV_REQUIRES freertos esp_timer esp_rom)