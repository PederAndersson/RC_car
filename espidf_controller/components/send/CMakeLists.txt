idf_component_register (
    SRCS "send.cpp"
    INCLUDE_DIRS "include"
    REQUIRES nvs_flash esp_wifi esp_event esp_netif freertos
)