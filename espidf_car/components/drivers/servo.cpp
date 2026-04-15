#include <algorithm>
#include "servo.h"
#include "driver/ledc.h"
#include "esp_check.h"
#include "esp_err.h"

static const char* TAG = "Servo";
namespace Driver {

    Sg90::Sg90(const ServoConfig& config) :
        config_(config),
        initialized_(false),
        current_angle_(90)
    
    {}

    esp_err_t Sg90::init(){
        esp_err_t err = gpio_set_direction(config_.pwm_pin, GPIO_MODE_OUTPUT);
        ESP_RETURN_ON_ERROR(err, TAG, "gpio_set_direction failed");

        // Configure LEDC timer/channel for a 50 Hz servo PWM signal.
        ledc_timer_config_t timer_cfg {
            .speed_mode = config_.mode,
            .duty_resolution = config_.duty_res,
            .timer_num = config_.timer,
            .freq_hz = config_.pwm_freq_hz,
            .clk_cfg = LEDC_AUTO_CLK,
            .deconfigure = false
        };
        err = ledc_timer_config(&timer_cfg);
        ESP_RETURN_ON_ERROR(err, TAG, "ledc_timer_config() failed");
        ledc_channel_config_t channel_cfg {
            .gpio_num = config_.pwm_pin,
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
        ESP_RETURN_ON_ERROR(err, TAG, "ledc_channel_config failed");
        initialized_ = true;
        // Center on boot so steering starts from a predictable neutral position.
        err = set_angle(90);
        ESP_RETURN_ON_ERROR(err,TAG,"set_angle failed");
        return ESP_OK;
    }

    esp_err_t Sg90::set_angle(uint8_t angle){
        if (!initialized_) {return ESP_ERR_INVALID_STATE;}
        if (config_.pwm_freq_hz == 0 || config_.max_angle <= config_.min_angle) {return ESP_ERR_INVALID_ARG;}
        if (config_.max_pulse_us <= config_.min_pulse_us) {return ESP_ERR_INVALID_ARG;}

        // Convert target angle -> pulse width (us) -> LEDC duty counts.
        const uint32_t period_us = 1000000u / config_.pwm_freq_hz;
        const uint32_t max_duty = (1u << static_cast<uint32_t>(config_.duty_res)) -1u;
        const uint32_t angle_clamp = std::min<uint32_t>(config_.max_angle,
             std::max<uint32_t>(config_.min_angle, angle));
        const uint32_t pulse_us = config_.min_pulse_us + 
                    ((angle_clamp - config_.min_angle)*(config_.max_pulse_us - config_.min_pulse_us)) /
                    (config_.max_angle - config_.min_angle);
        const uint32_t duty = ((pulse_us * max_duty + (period_us / 2u)) / period_us);
        esp_err_t err = ledc_set_duty(config_.mode, config_.channel, duty);
        ESP_RETURN_ON_ERROR(err,TAG, "ledc_set_duty failed");
        err = ledc_update_duty(config_.mode, config_.channel);
        ESP_RETURN_ON_ERROR(err,TAG,"ledc_update_duty failed");
        current_angle_ = angle_clamp;
        return ESP_OK;
    }
}