#include "control.h"

static const char *TAG = "control";

//y-avg = 2123 x-avg = 2172 

void joystick_to_car_input(Car_input *input, Joystick *joystick) {
    const uint32_t X_CENTER = 2172;
    const uint32_t Y_CENTER = 2123;
    const uint8_t DEADZONE = 100;
    if (joystick->x_axis >= (X_CENTER - DEADZONE) && joystick->x_axis <= (X_CENTER + DEADZONE)) {input->steering = 90;}
    else {}
    if (joystick->y_axis >= (Y_CENTER - DEADZONE) && joystick->y_axis <= (Y_CENTER + DEADZONE)) {input->throttle = 0;}
    else{
        
    }

}