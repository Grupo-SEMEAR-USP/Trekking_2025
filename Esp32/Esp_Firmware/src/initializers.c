#include "initializers.h"

ledc_timer_config_t ledc_timer_config_info(ledc_timer_t timer_id){
    
    ledc_timer_config_t pwm_timer = {

        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .timer_num = timer_id,
        .freq_hz = 2000,
        .clk_cfg = LEDC_AUTO_CLK

    };

    return pwm_timer;
}
//
ledc_timer_config_t ledc_servo_timer_config_info(ledc_timer_t timer_id){
    
    ledc_timer_config_t pwm_timer = {

        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .timer_num = timer_id,
        .freq_hz = 2000,
        .clk_cfg = LEDC_AUTO_CLK

    };

    return pwm_timer;
}

servo_config_t servo_config_info(ledc_timer_t timer_id,ledc_channel_t channel,gpio_num_t servo_pwm_pin){

    servo_config_t servo_cfg = {
    .max_angle = 180,
    .min_width_us = 500,
    .max_width_us = 2500,
    .freq = 50,
    .timer_number = timer_id,
    .channels = {
        .servo_pin = {
            servo_pwm_pin
        },
        .ch = {
            channel
        },
    },
    .channel_number = 1,
} ;

    return servo_cfg;
}

//
ledc_channel_config_t ledc_channel_config_info(
    gpio_num_t pwm_pin,
    ledc_channel_t channel,
    ledc_timer_t timer_id){

    ledc_channel_config_t pwm_channel = {
        .gpio_num = (int) pwm_pin,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = channel,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = timer_id,
        .duty = 0,
        .hpoint = 0,
        .flags = {.output_invert = 0}
    };

    return pwm_channel;
}

//pid initializers

pid_ctrl_config_t pid_ctrl_config_info(float *k_values,float max_duty_value, float max_integral){

    pid_ctrl_parameter_t config_parameter = {
        .kp = k_values[0],
        .ki = k_values[1],
        .kd = k_values[2],
        .max_output = max_duty_value,
        .min_output = -max_duty_value,
        .max_integral = max_integral,
        .min_integral = -max_integral,
        //.cal_type = PID_CAL_TYPE_POSITIONAL_DELTA_TIME, //se tiver dando merda coloque PID_CAL_TYPE_POSITIONAL_DEAFULT
        .cal_type = PID_CAL_TYPE_POSITIONAL_DEFAULT,//PID_CAL_TYPE_POSITIONAL,
        .prev_ts = 0
    };

    pid_ctrl_config_t config;
    config.init_param = config_parameter;

    return config;

}

// Testando sem os pull_ups internos da esp
//i2c initializers
i2c_config_t i2c_slave_config_info(
    gpio_num_t sda_pin,
    gpio_num_t slc_pin,
    uint16_t slave_addr){

    i2c_config_t conf_slave;
        conf_slave.mode = I2C_MODE_SLAVE;
        conf_slave.sda_io_num = (int) sda_pin;
        conf_slave.scl_io_num = (int) slc_pin;          
        conf_slave.sda_pullup_en = GPIO_PULLUP_DISABLE;           
        conf_slave.scl_pullup_en = GPIO_PULLUP_DISABLE;
        conf_slave.slave.addr_10bit_en = 0;
        conf_slave.slave.slave_addr = slave_addr;       
        conf_slave.slave.maximum_speed = 100000;
        conf_slave.clk_flags = 0;

    return  conf_slave;                          
    
}

//gpio pin default config
gpio_config_t gpio_default_config = {
    .pin_bit_mask = (1ULL << INPUT_1) | 
                    (1ULL << INPUT_2) | 
                    (1ULL << INPUT_3) | 
                    (1ULL << INPUT_4),
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_ENABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE
};


