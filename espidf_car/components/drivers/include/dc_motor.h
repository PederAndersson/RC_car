#ifndef DC_MOTOR_H
#define DC_MOTOR_H

#include "soc/gpio_num.h"
#include <stdint.h>
namespace DC {

    struct MotorPins {
        gpio_num_t  enable_pin;
        gpio_num_t  in1;
        gpio_num_t  in2;
    };

    struct MotorConfig {
        MotorPins motor;
        gpio_num_t ledc_channel;
        gpio_num_t ledc_timer;
        gpio_num_t pwm_freq_hz;
        gpio_num_t pwm_resolution;
    };

    enum class MotorState {
        Idle,
        Forward,
        Reverse,
        Brake,
        Error
        
    };

    enum class StopMode {
        Brake,
        Coast
    };
    class DcMotor {
        public:

        void init();
        void set_throttle(int8_t percent);
        void coast_stop();
        void brake_stop();
        void state();

        private:
        struct Motor;
        bool apply_direction(int32_t joystick_input);
        uint8_t set_pwm(int32_t joystick_input);

    };

}


#endif