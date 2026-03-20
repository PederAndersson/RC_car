#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"


static const char* TAG = "RC_CAR";

extern "C" void app_main(void) {
    ESP_LOGI(TAG, "RC car boot ok");

    while (true){
        ESP_LOGI(TAG, "alive");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}