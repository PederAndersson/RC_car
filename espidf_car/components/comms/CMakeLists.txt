idf_component_register(
    SRCS "esp_now_receive.cpp"
    INCLUDE_DIRS "include"
    REQUIRES nvs_flash esp_wifi esp_event esp_netif freertos
)