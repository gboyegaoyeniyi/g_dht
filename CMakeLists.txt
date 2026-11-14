idf_component_register(SRCS "g_dht.c"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES freertos driver esp_timer)