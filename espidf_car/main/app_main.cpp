#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "dc_motor.h"

const Driver::MotorConfig motor_config{
    .motor = {
        .enable_gpio = GPIO_NUM_10,
        .in1_gpio = GPIO_NUM_6,
        .in2_gpio = GPIO_NUM_7,
    },
    .timer = LEDC_TIMER_0,
    .channel = LEDC_CHANNEL_0,
    .mode = LEDC_LOW_SPEED_MODE,
    .duty_res = LEDC_TIMER_10_BIT,
    .pwm_freq_hz = 1000
};

Driver::L298N motor(motor_config);


static const char* TAG = "RC_CAR";

extern "C" void app_main(void) {
    ESP_LOGI(TAG, "RC car boot ok");
    esp_err_t err = motor.init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "motor.init() failed : %s", esp_err_to_name(err));
        return;
    }
    while (true) {
        ESP_LOGI(TAG, "Forward 75%%");
        err = motor.set_throttle_percent(75);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "set_throttle_percent failed: %s", esp_err_to_name(err));
        }
        vTaskDelay(pdMS_TO_TICKS(2000));

        ESP_LOGI(TAG, "Stop");
        err = motor.set_throttle_percent(0);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "set_throttle_percent failed: %s", esp_err_to_name(err));
        }
        vTaskDelay(pdMS_TO_TICKS(1000));

        ESP_LOGI(TAG, "Reverse 75%%");
        err = motor.set_throttle_percent(-75);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "set_throttle_percent failed: %s", esp_err_to_name(err));
        }
        vTaskDelay(pdMS_TO_TICKS(2000));

        ESP_LOGI(TAG, "Stop");
        err = motor.set_throttle_percent(0);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "set_throttle_percent failed: %s", esp_err_to_name(err));
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}