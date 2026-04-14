#include "esp_err.h"
#include "esp_now_receive.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "dc_motor.h"
#include "hal/ledc_types.h"
#include "servo.h"
#include "esp_check.h"
#include "esp_mac.h"
// Hardware-to-driver mapping for the L298N motor channel.
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

// Typical SG90 setup: 50 Hz frame and calibrated pulse window.
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

Car_input car{
    .throttle = 0,
    .steering = 90
};

static const char* TAG = "RC_CAR";

extern "C" esp_err_t app_main(void) {
    ESP_LOGI(TAG, "RC car boot ok");

    // Initialize peripherals once before entering the control loop.
    esp_err_t err = motor.init();
    ESP_RETURN_ON_ERROR(err, TAG, "motor.init failed");
    err = servo.init();
    ESP_RETURN_ON_ERROR(err,TAG,"servo.init failed");
    err = car_control_init();
    ESP_RETURN_ON_ERROR(err, TAG, "car_control_init failed");
    uint8_t mac[6];
    err = esp_wifi_get_mac(WIFI_IF_STA, mac);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to get MAC-adress");
    ESP_LOGI(TAG, "Car MAC: " MACSTR, MAC2STR(mac));
    while (true) {
        err = receive_input(&car, 500);
        if (err != ESP_ERR_TIMEOUT){
        motor.set_throttle_percent(car.throttle);
        servo.set_angle(car.steering);
    } else {
        motor.set_throttle_percent(0);
    }

    }

}