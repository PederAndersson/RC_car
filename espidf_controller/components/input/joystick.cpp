#include "joystick.h"
#include "esp_adc/adc_oneshot.h"
#include "hal/adc_types.h"
#include "soc/clk_tree_defs.h"
#include "esp_check.h"

static const char* TAG = "joystick";
static adc_oneshot_unit_handle_t joystick_handle;

esp_err_t joystick_init(){
    adc_oneshot_unit_init_cfg_t ADC_CFG1{
        .unit_id = ADC_UNIT_1,
        .clk_src = ADC_DIGI_CLK_SRC_DEFAULT,
    };
    
    ESP_RETURN_ON_ERROR(adc_oneshot_new_unit(&ADC_CFG1, &joystick_handle), TAG, "new_unit failed");
    adc_oneshot_chan_cfg_t joystick_cfg {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT
    };
    ESP_RETURN_ON_ERROR(adc_oneshot_config_channel(joystick_handle, ADC_CHANNEL_4, &joystick_cfg), TAG, "adc_channel_cofig failed");
    ESP_RETURN_ON_ERROR(adc_oneshot_config_channel(joystick_handle, ADC_CHANNEL_3, &joystick_cfg), TAG, "adc_channel_cofig failed");
    
    return ESP_OK;
}

esp_err_t read_joystick(Joystick *joystick) {

    ESP_RETURN_ON_ERROR(adc_oneshot_read(joystick_handle, ADC_CHANNEL_3, &joystick->x_axis), TAG, "adc_read failed");
    ESP_RETURN_ON_ERROR(adc_oneshot_read(joystick_handle, ADC_CHANNEL_4, &joystick->y_axis), TAG, "adc_read failed");
    return ESP_OK;
}