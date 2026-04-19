#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "esp_err.h"

typedef struct {
    int x_axis;
    int y_axis;
}Joystick;

esp_err_t joystick_init();
esp_err_t read_joystick(Joystick *joystick);



#endif