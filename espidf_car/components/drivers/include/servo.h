#ifndef SERVO_H
#define SERVO_H

#include "esp_err.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "hal/ledc_types.h"

namespace Driver {

    struct ServoConfig{
        gpio_num_t pwm_pin; //pin 5
        ledc_timer_t timer;
        ledc_channel_t channel;
        ledc_mode_t mode;
        ledc_timer_bit_t duty_res;
        uint32_t pwm_freq_hz;
        uint32_t min_pulse_us;
        uint32_t max_pulse_us;
        uint8_t min_angle;
        uint8_t max_angle;

    };

    class Sg90 {
        public:
        explicit Sg90(const ServoConfig& servo);
        ~Sg90() = default;
        esp_err_t init();
        esp_err_t set_angle(uint8_t angle);
        private:
        ServoConfig config_;
        bool initialized_;
        uint8_t current_angle_;
    };
}


#endif