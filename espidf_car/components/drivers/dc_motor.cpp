#include "dc_motor.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "soc/clk_tree_defs.h"

namespace Driver {

    L298N::L298N(const MotorConfig& config) :
        config_(config),
        initialized_(false),
        direction_(Direction::Forward),
        speed_percent_(0)
    {}

    esp_err_t L298N::init(){
        // IN1/IN2 are the H-bridge direction pins; start both low (coast).
        esp_err_t err = gpio_set_direction(config_.motor.in1_gpio, GPIO_MODE_OUTPUT);
        if (err != ESP_OK) {return err;}
        err = gpio_set_direction(config_.motor.in2_gpio, GPIO_MODE_OUTPUT);
        if (err != ESP_OK) {return err;}
        err = gpio_set_level(config_.motor.in1_gpio, 0);
        if (err != ESP_OK) {return err;}
        err = gpio_set_level(config_.motor.in2_gpio, 0);
        if (err != ESP_OK) {return err;}

        ledc_timer_config_t timer_cfg {
            .speed_mode = config_.mode,
            .duty_resolution = config_.duty_res,
            .timer_num = config_.timer,
            .freq_hz = config_.pwm_freq_hz,
            .clk_cfg = LEDC_AUTO_CLK,
            .deconfigure = false
        };

        err = ledc_timer_config(&timer_cfg);
        if (err != ESP_OK) {return err;}

        ledc_channel_config_t channel_cfg {
            .gpio_num = config_.motor.enable_gpio,
            .speed_mode = config_.mode,
            .channel = config_.channel,
            .intr_type = LEDC_INTR_DISABLE,
            .timer_sel = config_.timer,
            .duty = 0,
            .hpoint = 0,
            .sleep_mode = LEDC_SLEEP_MODE_NO_ALIVE_NO_PD,
            .flags = {0}
        };

        err = ledc_channel_config(&channel_cfg);
        if (err != ESP_OK) {return err;}
        initialized_ = true;

        return ESP_OK;
    }

    esp_err_t L298N::set_speed_percent(uint8_t percent){
        if (!initialized_) {return ESP_ERR_INVALID_STATE;}
        if (percent > 100) {percent = 100;}
        // LEDC duty is an N-bit value in range [0, 2^N - 1].
        const uint32_t max_duty = (1u << static_cast<uint32_t>(config_.duty_res)) - 1u;
        const uint32_t duty = (static_cast<uint32_t>(percent) * max_duty) / 100u;

        esp_err_t err = ledc_set_duty(config_.mode, config_.channel, duty);
        if (err != ESP_OK) {return err;}
        err = ledc_update_duty(config_.mode, config_.channel);
        if(err != ESP_OK) {return err;}
        speed_percent_ = percent;
        return ESP_OK;
    }

    esp_err_t L298N::set_direction(Direction direction){
        if(!initialized_) {return ESP_ERR_INVALID_STATE;}
        esp_err_t err;
        // L298N truth table for one motor channel: IN1=1,IN2=0 forward; IN1=0,IN2=1 reverse.
        if (direction == Direction::Forward){
            err = gpio_set_level(config_.motor.in1_gpio, 1);
            if (err != ESP_OK) {return err;}
            err = gpio_set_level(config_.motor.in2_gpio, 0);
            if (err != ESP_OK) {return err;}
            direction_ = Direction::Forward;
        }
        else if (direction == Direction::Reverse){
            err = gpio_set_level(config_.motor.in1_gpio, 0);
            if (err != ESP_OK) {return err;}
            err = gpio_set_level(config_.motor.in2_gpio, 1);
            if (err != ESP_OK) {return err;}
            direction_ = Direction::Reverse;
        }
        else {
            return ESP_ERR_INVALID_ARG;
        }
        return ESP_OK;
    }

    esp_err_t L298N::stop(StopMode stop){
        if (!initialized_) {return ESP_ERR_INVALID_STATE;}
        esp_err_t err = set_speed_percent(0);
        if (err != ESP_OK) {return err;}
        switch (stop){
            case StopMode::Coast:{
                // Coast: both low lets motor spin down freely.
                err = gpio_set_level(config_.motor.in1_gpio,0);
                if (err != ESP_OK){return err;}
                err = gpio_set_level(config_.motor.in2_gpio,0);
                if (err !=ESP_OK){return err;}
                break;
            }
            case StopMode::Brake:{
                // Brake: both high actively short-brakes the motor through the bridge.
                err = gpio_set_level(config_.motor.in1_gpio,1);
                if (err != ESP_OK){return err;}
                err = gpio_set_level(config_.motor.in2_gpio,1);
                if(err != ESP_OK){return err;}
                break;
            }
            default:{return ESP_ERR_INVALID_ARG;break;}
        }
        return ESP_OK;
    }

    esp_err_t L298N::set_throttle_percent(int8_t percent){
        if (!initialized_) {return ESP_ERR_INVALID_STATE;}
        if (percent < -100) {percent = -100;}
        if (percent > 100) {percent = 100;}
        // Signed throttle maps to direction (sign) + magnitude (absolute value).
        if (percent == 0) {return stop(StopMode::Coast);}
        esp_err_t err = set_direction(percent > 0 ? Direction::Forward : Direction::Reverse);
        if (err != ESP_OK) {return err;}

        return set_speed_percent(static_cast<uint8_t>(percent > 0 ? percent : -percent));
    }
}