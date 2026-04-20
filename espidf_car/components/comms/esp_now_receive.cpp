#include "esp_now_receive.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_now.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

static const char *TAG = "ESP_NOW";
// Single-slot queue keeps only the newest command (low latency over full history).
static QueueHandle_t received_data_queue;

static void esp_now_receive_callback(const esp_now_recv_info_t *info, const uint8_t *data, int data_len){
    (void)info;
    // Only accept payloads that exactly match our wire format.
    if (data_len != sizeof(Car_input)){
        ESP_LOGE(TAG, "Wrong data");
        return;
    }
    ESP_LOGI(TAG, "packet received, len=%d", data_len);
    const Car_input *cmd = (const Car_input *)data;
    xQueueOverwrite(received_data_queue, cmd);
}

esp_err_t receive_input(Car_input *input, uint32_t timeout){
    // Convert milliseconds to RTOS ticks for deterministic timeout handling.
    BaseType_t ok = xQueueReceive(received_data_queue, input, pdMS_TO_TICKS(timeout));
    if (ok != pdTRUE){return ESP_ERR_TIMEOUT;}
    return ESP_OK;
    
}

esp_err_t car_control_init(){
    // ESP-NOW depends on Wi-Fi, and Wi-Fi requires NVS + netif + event loop first.
    // Initialize non-volatile storage used by Wi-Fi/ESP-NOW to persist calibration/config data.
    esp_err_t err = nvs_flash_init();
    ESP_RETURN_ON_ERROR(err, TAG, "nvs_flash_init failed");

    // Bring up TCP/IP network interfaces and core network stack objects.
    err = esp_netif_init();
    ESP_RETURN_ON_ERROR(err, TAG, "esp_netif_init failed");

    // Create the default event loop so system/network events can be dispatched.
    err = esp_event_loop_create_default();
    ESP_RETURN_ON_ERROR(err, TAG, "event_loop_create failed");

    // Fill Wi-Fi config with recommended default settings from ESP-IDF.
    wifi_init_config_t wifi_cfg = WIFI_INIT_CONFIG_DEFAULT();

    // Initialize the Wi-Fi driver with the selected configuration.
    err = esp_wifi_init(&wifi_cfg);
    ESP_RETURN_ON_ERROR(err,TAG,"wifi_init failed");

    // Use station mode (required for ESP-NOW peer-to-peer operation here).
    err = esp_wifi_set_mode(WIFI_MODE_STA);
    ESP_RETURN_ON_ERROR(err, TAG, "wifi_set_mode failed");

    // Start the Wi-Fi driver so radio and internal tasks are running.
    err = esp_wifi_start();
    ESP_RETURN_ON_ERROR(err, TAG, "wifi_start failed");

    // Initialize ESP-NOW protocol layer on top of the active Wi-Fi stack.
    err = esp_now_init();
    ESP_RETURN_ON_ERROR(err, TAG, "esp_now_init failed");

    // Create a one-item queue that always stores only the latest control command.
    received_data_queue = xQueueCreate(1, sizeof(Car_input));
    if (received_data_queue == NULL){
        ESP_LOGE(TAG, "Queue create failed.");
        return ESP_ERR_NO_MEM;
    }

    // Register callback invoked whenever an ESP-NOW packet is received.
    err = esp_now_register_recv_cb(esp_now_receive_callback);
    ESP_RETURN_ON_ERROR(err, TAG, "register_recv_cb failed");

    return err;
}
