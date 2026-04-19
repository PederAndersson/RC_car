#ifndef CONTROL_H
#define CONTROL_H

#include "send.h"
#include "joystick.h"

void joystick_to_car_input(Car_input *input, Joystick *joystick);

#endif