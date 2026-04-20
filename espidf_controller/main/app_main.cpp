#include <cstdio>
#include <stdio.h>
#include "control.h"
#include "freertos/projdefs.h"
#include "joystick.h"
#include "send.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "controller";

extern "C" void app_main(void) {
    ESP_LOGI(TAG, "Controller boot ok");
    ESP_RETURN_VOID_ON_ERROR(controller_init(), TAG, "controller init failed");
    ESP_RETURN_VOID_ON_ERROR(joystick_init(), TAG, "joytsick init failed");
    Car_input input{};
    Joystick joystick{};

    input.steering = 90;
    input.throttle = 50;

    while(true){
        read_joystick(&joystick);
        joystick_to_car_input(&input, &joystick);
        send_input(&input);
        vTaskDelay(pdMS_TO_TICKS(20));

    }

}
