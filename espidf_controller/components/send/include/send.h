#ifndef SEND_H
#define SEND_H

#include <stdint.h>
#include "esp_err.h"

typedef struct __attribute__((packed)) {
    int8_t throttle;
    uint8_t steering;
}Car_input;

esp_err_t controller_init();
esp_err_t send_input(Car_input *input);


#endif