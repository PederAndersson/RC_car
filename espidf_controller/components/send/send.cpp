#include "send.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_now.h"
#include <cstring>

static const char *TAG = "SEND";

namespace {
    constexpr uint8_t car_MAC[ESP_NOW_ETH_ALEN] = {0xdc,0xb4,0xd9,0x9a,0x61,0xf8};
}

esp_err_t send_input(Car_input *input){
    if (input == nullptr){return ESP_ERR_INVALID_ARG;}
    esp_err_t err = esp_now_send(car_MAC, (const uint8_t*)input, sizeof(Car_input));
    ESP_RETURN_ON_ERROR(err, TAG, "esp_now_send failed");
    return err;

}

esp_err_t controller_init(){
    esp_err_t err = nvs_flash_init();
    ESP_RETURN_ON_ERROR(err, TAG, "nvs_flash_init failed");
    err = esp_netif_init();
    ESP_RETURN_ON_ERROR(err, TAG, "netif_init failed");
    err = esp_event_loop_create_default();
    ESP_RETURN_ON_ERROR(err, TAG, "event_loop_create failed"); 
    wifi_init_config_t wifi_cfg = WIFI_INIT_CONFIG_DEFAULT();
    err = esp_wifi_init(&wifi_cfg);
    ESP_RETURN_ON_ERROR(err, TAG, "WiFi_init failed");
    err = esp_wifi_set_mode(WIFI_MODE_STA);
    ESP_RETURN_ON_ERROR(err, TAG, "wifi_set_mode failed");
    err = esp_wifi_start();
    ESP_RETURN_ON_ERROR(err, TAG, "wifi_start failed");
    err = esp_now_init();
    ESP_RETURN_ON_ERROR(err, TAG, "esp_now failed");
    esp_now_peer_info_t car{}; //zero-init all fields
    std::memcpy(car.peer_addr, car_MAC, ESP_NOW_ETH_ALEN);
    car.channel = 0; //use current channel
    car.encrypt = false; //no LMK encryption
    ESP_RETURN_ON_ERROR(esp_now_add_peer(&car), TAG, "add_peer failed");
    return err;
}