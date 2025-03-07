#include "pwm.h"
#include "initializers.h"

void pwm_motors_init(){
    //initializing pwm for left motor 
    ledc_timer_config_t timer_config = ledc_timer_config_info(timer_choose(ESQ));
    ledc_channel_config_t channel_config = ledc_channel_config_info(ENABLE_ESQ,channel_choose(ESQ),timer_choose(ESQ));
    
    ledc_timer_config(&timer_config);
    ledc_channel_config(&channel_config);

    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel_choose(ESQ), 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel_choose(ESQ));

    gpio_pad_select_gpio(motor_choose_in_primary(ESQ));
    gpio_set_direction(motor_choose_in_primary(ESQ), GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(motor_choose_in_secundary(ESQ));
    gpio_set_direction(motor_choose_in_secundary(ESQ), GPIO_MODE_OUTPUT);

    gpio_set_level(motor_choose_in_primary(ESQ), 0); 
    gpio_set_level(motor_choose_in_secundary(ESQ), 0);

    //initializing pwm for right motor
    timer_config = ledc_timer_config_info(timer_choose(DIR));
    channel_config = ledc_channel_config_info(ENABLE_DIR,channel_choose(DIR),timer_choose(DIR));

    ledc_timer_config(&timer_config);
    ledc_channel_config(&channel_config);

    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel_choose(DIR), 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel_choose(DIR));

    gpio_pad_select_gpio(motor_choose_in_primary(DIR));
    gpio_set_direction(motor_choose_in_primary(DIR), GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(motor_choose_in_secundary(DIR));
    gpio_set_direction(motor_choose_in_secundary(DIR), GPIO_MODE_OUTPUT);

    gpio_set_level(motor_choose_in_primary(DIR), 0); 
    gpio_set_level(motor_choose_in_secundary(DIR), 0);

    //initializing pwm for servo

    //timer_config = ledc_servo_timer_config_info(SERVO_PWM_TIMER);
    //channel_config = ledc_channel_config_info(SERVO_DUTY_PIN,SERVO_PWM_CHANNEL,SERVO_PWM_TIMER);

    //ledc_timer_config(&timer_config);
    //ledc_channel_config(&channel_config);

    //ledc_set_duty(LEDC_LOW_SPEED_MODE, SERVO_PWM_CHANNEL, 7000); 
    //ledc_update_duty(LEDC_LOW_SPEED_MODE, SERVO_PWM_CHANNEL);

    servo_config_t servo_config = servo_config_info(SERVO_PWM_TIMER,SERVO_PWM_CHANNEL,SERVO_DUTY_PIN);
//
    iot_servo_init(LEDC_LOW_SPEED_MODE, &servo_config);
    iot_servo_write_angle(LEDC_LOW_SPEED_MODE, SERVO_PWM_CHANNEL, 45); //86.5 =90 offset = -3.5
    //iot_servo_init(LEDC_LOW_SPEED_MODE, &servo_config);
//
    //float angle = 45.0f;
//
    //// Set angle to 100 degree
    //iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 0, angle);


}

void pwm_actuate(int channel, float duty){
    
    bool clockwise = (duty < 0);

    if(clockwise){
        duty *= -1;
    }

    //define rotation direction 
    gpio_set_level(motor_choose_in_primary(channel), (clockwise) ? 0 : 1); 
    gpio_set_level(motor_choose_in_secundary(channel), (clockwise) ? 1 : 0);

    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel_choose(channel), ((uint32_t) duty));
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel_choose(channel));
}
