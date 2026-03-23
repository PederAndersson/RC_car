#ifndef DC_MOTOR_H
#define DC_MOTOR_H

#include "esp_err.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "hal/ledc_types.h"
#include <stdint.h>


namespace Driver {
    
    struct MotorPins {
        gpio_num_t  enable_gpio;
        gpio_num_t  in1_gpio;
        gpio_num_t  in2_gpio;
    };
    struct MotorConfig {
        MotorPins motor;
        ledc_timer_t timer;
        ledc_channel_t channel;
        ledc_mode_t mode;
        ledc_timer_bit_t duty_res;
        uint32_t pwm_freq_hz;
    };


    enum class Direction {
        Forward,
        Reverse,
    };
    
    enum class StopMode {
        Brake,
        Coast
    };

    class L298N {
        public:
        explicit L298N(const MotorConfig& config);
        ~L298N() = default;
        esp_err_t init();
        esp_err_t set_throttle_percent(int8_t percent);
        esp_err_t set_speed_percent(uint8_t percent);
        esp_err_t set_direction(Direction direction);
        esp_err_t stop(StopMode mode);
        
        private:
        MotorConfig config_;
        bool initialized_;
        Direction direction_;
        uint8_t speed_percent_;
    };
    
}


#endif