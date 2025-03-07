#pragma once

#include "types.h"

//pwm initializers 

ledc_timer_config_t ledc_timer_config_info(ledc_timer_t timer_id);
 

ledc_channel_config_t ledc_channel_config_info(
    gpio_num_t pwm_pin,
    ledc_channel_t channel,
    ledc_timer_t timer_id);

//pid initializers

pid_ctrl_config_t pid_ctrl_config_info(float *k_values,float max_duty_value, float max_integral);

//i2c initializers

i2c_config_t i2c_slave_config_info(
    gpio_num_t sda_pin,
    gpio_num_t slc_pin,
    uint16_t slave_addr);

//gpio pin default config
extern gpio_config_t gpio_default_config;



