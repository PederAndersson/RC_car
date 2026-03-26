#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "dc_motor.h"
#include "hal/ledc_types.h"
#include "servo.h"
#include "esp_check.h"

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

const Driver::ServoConfig servo_cfg{
    .pwm_pin = GPIO_NUM_5,
    .timer = LEDC_TIMER_1,
    .channel = LEDC_CHANNEL_1,
    .mode = LEDC_LOW_SPEED_MODE,
    .duty_res = LEDC_TIMER_14_BIT,
    .pwm_freq_hz = 50,
    .min_pulse_us = 1000, //500
    .max_pulse_us = 2000, //2400
    .min_angle = 0,
    .max_angle = 180
};

Driver::L298N motor(motor_config);
Driver::Sg90 servo(servo_cfg);

static const char* TAG = "RC_CAR";

extern "C" esp_err_t app_main(void) {
    ESP_LOGI(TAG, "RC car boot ok");
    esp_err_t err = motor.init();
    ESP_RETURN_ON_ERROR(err, TAG, "motor.init failed");
    err = servo.init();
    ESP_RETURN_ON_ERROR(err,TAG,"servo.init failed");
    while (true) {
        ESP_LOGI(TAG, "Forward 75%%");
        err = motor.set_throttle_percent(75);
        ESP_RETURN_ON_ERROR(err,TAG,"motor.set_trottle failed");
        err = servo.set_angle(45);
        ESP_RETURN_ON_ERROR(err,TAG,"servo.set_angle failed");
        vTaskDelay(pdMS_TO_TICKS(2000));

        ESP_LOGI(TAG, "Stop");
        err = motor.set_throttle_percent(0);
        ESP_RETURN_ON_ERROR(err,TAG,"motor.set_throttle failed");
        err = servo.set_angle(90);
        ESP_RETURN_ON_ERROR(err,TAG,"servo.set_angle failed");
        vTaskDelay(pdMS_TO_TICKS(1000));

        ESP_LOGI(TAG, "Reverse 75%%");
        err = motor.set_throttle_percent(-75);
        ESP_RETURN_ON_ERROR(err,TAG,"motor.set_throttle failed");
        err = servo.set_angle(135);
        ESP_RETURN_ON_ERROR(err,TAG,"servo.set_angle failed");
        vTaskDelay(pdMS_TO_TICKS(2000));

        ESP_LOGI(TAG, "Stop");
        err = motor.set_throttle_percent(0);
        ESP_RETURN_ON_ERROR(err,TAG,"motor.set_throttle failed");
        err = servo.set_angle(90);
        ESP_RETURN_ON_ERROR(err,TAG,"servo.set_angle failed");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}