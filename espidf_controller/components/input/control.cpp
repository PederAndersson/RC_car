#include "control.h"
//y-avg = 2123 x-avg = 2172 

void joystick_to_car_input(Car_input *input, Joystick *joystick) {
    const int32_t X_CENTER = 2172;
    const int32_t Y_CENTER = 2123;
    const uint8_t DEADZONE = 100;
    if (joystick->x_axis >= (X_CENTER - DEADZONE) && joystick->x_axis <= (X_CENTER + DEADZONE)) {input->steering = 90;}
    else {
        int32_t offset = (int32_t)joystick->x_axis - X_CENTER;
        int32_t angle;
        if (offset >= 0){
            angle = 90 + (offset * 90 / (4095 - X_CENTER));
        }else {
            angle = 90 + (offset * 90 / X_CENTER);
        }
        if (angle > 180) {angle = 180;}
        if (angle < 0) {angle = 0;}
        input->steering = (uint8_t)angle;

    }
    if (joystick->y_axis >= (Y_CENTER - DEADZONE) && joystick->y_axis <= (Y_CENTER + DEADZONE)) {input->throttle = 0;}
    else{
        int32_t offset = (int32_t)joystick->y_axis - Y_CENTER;
        int32_t throttle = 0;
        if (offset >= 0){
            throttle = offset * 100 / (4095 - Y_CENTER);
        }else if (offset < 0) {
            throttle = (offset * 100 / Y_CENTER);
        }
        if (throttle > 100) {throttle = 100;}
        if (throttle < -100) {throttle = -100;}
        input->throttle = (int8_t)throttle;

    }

}