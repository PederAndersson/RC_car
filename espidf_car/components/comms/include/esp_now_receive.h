#ifndef ESP_NOW_RECEIVE_H
#define ESP_NOW_RECEIVE_H

#include <stdint.h>
#include "esp_err.h"

enum status {
    CONNECTING,
    CONNECTED,
    DISCONNECTED
};

// Packed to guarantee a stable byte layout over the wireless link.
typedef struct __attribute__((packed)) {
    int8_t throttle;
    uint8_t steering;
}Car_input;

// Initializes NVS, Wi-Fi (STA), ESP-NOW and the receive queue/callback.
esp_err_t car_control_init();
// Waits for next command packet; returns ESP_ERR_TIMEOUT if none arrives in time.
esp_err_t receive_input(Car_input *input, uint32_t timeout);
#endif