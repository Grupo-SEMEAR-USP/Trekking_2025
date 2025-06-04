#include "task_core0.h"
#include "initializers.h"
#include "sycronization.h"
#include "global_variables.h"
#include "pwm.h"
#include "pid.h"

//variables for intializing encoder
rotary_encoder_t *encoder_left;
rotary_encoder_t *encoder_right;

//variables for initializing pid
pid_handle_v2 pid_handle_left;
pid_handle_v2 pid_handle_right;

float pid_result_duty_left;
float pid_result_duty_right;

float local_ros_angular_speed_left;
float local_ros_angular_speed_right;

float local_motor_angular_speed_left; 
float local_motor_angular_speed_right;

int count_get_real;
int count_get_ros;
int count_servo;

bool aux;

//variables for servo
float local_servo_angle;

//timer function to read encoder and calculate pid
void monitor_encoder_pid_calc(void *params);

//create timer handle
TimerHandle_t monitor_encoder_pid_calc_timer_handle;

//start motor interrupt function
static void IRAM_ATTR monitor_encoder_pid_calc_start(void *args);
static void IRAM_ATTR monitor_encoder_pid_calc_stop(void *args);

void interrupts_init();

void core0fuctions(void *params){

    pwm_motors_init();

    //selecting pcnt units
    uint32_t pcnt_unit_0 = 0;
    uint32_t pcnt_unit_1 = 1;

    // Create rotary encoder instances
    rotary_encoder_config_t config_encoder_left = ROTARY_ENCODER_DEFAULT_CONFIG((rotary_encoder_dev_t)pcnt_unit_0, PCNT_CHA_LEFT, PCNT_CHB_LEFT);
    encoder_left = NULL;
    ESP_ERROR_CHECK(rotary_encoder_new_ec11(&config_encoder_left, &encoder_left));

    rotary_encoder_config_t config_encoder_right = ROTARY_ENCODER_DEFAULT_CONFIG((rotary_encoder_dev_t)pcnt_unit_1, PCNT_CHA_RIGHT, PCNT_CHB_RIGHT);
    encoder_right = NULL;
    ESP_ERROR_CHECK(rotary_encoder_new_ec11(&config_encoder_right, &encoder_right));

    // Filter out glitch (1us)
    ESP_ERROR_CHECK(encoder_left->set_glitch_filter(encoder_left, 1));
    ESP_ERROR_CHECK(encoder_right->set_glitch_filter(encoder_right, 1));

    // Start encoder
    ESP_ERROR_CHECK(encoder_left->start(encoder_left));
    ESP_ERROR_CHECK(encoder_right->start(encoder_right));
    
    //initializing pid

    pid_result_duty_left = 0;
    pid_result_duty_right = 0;

    local_ros_angular_speed_left = 0;
    local_ros_angular_speed_right = 0;

    local_motor_angular_speed_left = 0; 
    local_motor_angular_speed_right = 0;

    pid_init(&pid_handle_left,&pid_handle_right);
    
    local_servo_angle = (float) SERVO_INITIAL_ANGLE;

    count_get_real = 0;
    count_get_ros = 0;
    count_servo = 0;

    aux = pdTRUE;

    xEventGroupSetBits(initialization_groupEvent, task0_init_done);
    xEventGroupWaitBits(initialization_groupEvent, task1_init_done, true, true, portMAX_DELAY);
    
    //create timer for reading encoder and calculating pid
    monitor_encoder_pid_calc_timer_handle = xTimerCreate("Timer do encoder e pid",pdMS_TO_TICKS(PID_DELAY),pdTRUE,NULL,monitor_encoder_pid_calc);

    //init interrupts
    //interrupts_init();
    xTimerStart(
    monitor_encoder_pid_calc_timer_handle 
    ,0);
    vTaskSuspend(NULL);

    //vTaskDelay(pdMS_TO_TICKS(10000000));


}
int cycle_count = 0;
void monitor_encoder_pid_calc(void *params){

    /*
    if(count_get_real == ENCODER_COUNTER_WAIT_PID_OP){
        xSemaphoreTake(xSemaphore_getSpeed,portMAX_DELAY);
        //global_motor_angular_speed_left = ((float) encoder_left->get_counter_value(encoder_left))*(ENCODER_RESOLUTION/((float)count));
        //global_motor_angular_speed_right = ((float) encoder_right->get_counter_value(encoder_right))*(ENCODER_RESOLUTION/((float)count));
        global_motor_angular_speed_left = ((float) encoder_left->get_counter_value(encoder_left))*ENCODER_RESOLUTION;
        global_motor_angular_speed_right = ((float) encoder_right->get_counter_value(encoder_right))*ENCODER_RESOLUTION;
        local_motor_angular_speed_left = global_motor_angular_speed_left;
        local_motor_angular_speed_right = global_motor_angular_speed_right;
        
        encoder_left->reset_counter_value(encoder_left);
        encoder_right->reset_counter_value(encoder_right);
        
        xSemaphoreGive(xSemaphore_getSpeed);
    
        count_get_real = 0;
    }

    if(count_get_ros == GET_ROS_VAL_COUNTER_WAIT_PID_OP){
        xSemaphoreTake(xSemaphore_getRosSpeed,portMAX_DELAY); //could be incremented in the first lock

        local_ros_angular_speed_left = global_ros_angular_speed_left;
        local_ros_angular_speed_right = global_ros_angular_speed_right;
        
        local_servo_angle = global_ros_servo_angle;
        
        xSemaphoreGive(xSemaphore_getRosSpeed);
        
        count_get_ros = 0;
    }
    */

    pid_handle_left.pid_calculate(
        &pid_handle_left,
        local_motor_angular_speed_left,
        local_ros_angular_speed_left,
        &pid_result_duty_left
    );

    pid_handle_right.pid_calculate(
        &pid_handle_right,
        local_motor_angular_speed_right,
        local_ros_angular_speed_right,
        &pid_result_duty_right
    );

    //printf("left_vel_calc: %f rad/s left_vel ros: %f rad/s left_duty: %f\n",local_motor_angular_speed_left,local_ros_angular_speed_left,pid_result_duty_left);
    //printf("right_vel_calc: %f rad/s right_vel ros: %f rad/s right_duty: %f\n",local_motor_angular_speed_right,local_ros_angular_speed_right,pid_result_duty_right);
    //pwm_actuate(ESQ,pid_result_duty_left);
    //pwm_actuate(DIR,pid_result_duty_right);

/*
    if (aux && cycle_count >= 10){
        if (count_servo < 30){
            xSemaphoreTake(xSemaphore_getRosSpeed,portMAX_DELAY); //could be incremented in the first lock
            
            local_servo_angle++;
            
            xSemaphoreGive(xSemaphore_getRosSpeed);

            count_servo++;
        } else {aux = pdFALSE;}

        cycle_count = 0;
    }

    if (!aux && cycle_count >= 10){
        if (count_servo > -30){
            xSemaphoreTake(xSemaphore_getRosSpeed,portMAX_DELAY); //could be incremented in the first lock

            local_servo_angle--;
            
            xSemaphoreGive(xSemaphore_getRosSpeed);
            count_servo--;
        } else {aux = pdTRUE;}
        cycle_count = 0;  
    }
   */
    iot_servo_write_angle(LEDC_LOW_SPEED_MODE, SERVO_PWM_CHANNEL, (local_servo_angle + SERVO_OFFSET));

    printf("ângulo do servo: %f\n", local_servo_angle);
    
    count_get_real++;
    count_get_ros++;
    cycle_count++;

}

static void IRAM_ATTR monitor_encoder_pid_calc_start(void *args)
{   
    //starting encoder and pid timer 

    xTimerStart(
        monitor_encoder_pid_calc_timer_handle 
        ,0);

    gpio_intr_disable(START_MOTOR_INTERRUPT_PIN);
    
}

static void IRAM_ATTR monitor_encoder_pid_calc_stop(void *args)
{   
    //stopping encoder and pid timer 

    xTimerStop(
        monitor_encoder_pid_calc_timer_handle 
        ,0);

    //setting motors speed to zero
    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel_choose(ESQ), 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel_choose(ESQ));

    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel_choose(DIR), 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel_choose(DIR));

    gpio_intr_disable(START_MOTOR_INTERRUPT_PIN);
    
}

void interrupts_init(){
    //start interrupt pin
    gpio_pad_select_gpio(START_MOTOR_INTERRUPT_PIN);
    gpio_set_direction(START_MOTOR_INTERRUPT_PIN, GPIO_MODE_INPUT);

    //stop interrupt pin
    gpio_pad_select_gpio(STOP_MOTOR_INTERRUPT_PIN);
    gpio_set_direction(STOP_MOTOR_INTERRUPT_PIN, GPIO_MODE_INPUT);

    //disable pullup and pulldown for start pin
    gpio_pullup_dis(START_MOTOR_INTERRUPT_PIN);
    gpio_pulldown_dis(START_MOTOR_INTERRUPT_PIN);

    //disable pullup and pulldown for stop pin
    gpio_pullup_dis(STOP_MOTOR_INTERRUPT_PIN);
    gpio_pulldown_dis(STOP_MOTOR_INTERRUPT_PIN);

    //installing gpio isr interrupt
    gpio_install_isr_service(0);

    //install interrupt for start pin
    //gpio_set_intr_type(START_MOTOR_INTERRUPT_PIN, GPIO_INTR_POSEDGE);
    //gpio_isr_handler_add(START_MOTOR_INTERRUPT_PIN, monitor_encoder_pid_calc_start, (void *)START_MOTOR_INTERRUPT_PIN);

    //install interrupt for stop pin
    gpio_set_intr_type(STOP_MOTOR_INTERRUPT_PIN, GPIO_INTR_POSEDGE);
    gpio_isr_handler_add(STOP_MOTOR_INTERRUPT_PIN, monitor_encoder_pid_calc_stop, (void *)STOP_MOTOR_INTERRUPT_PIN);

}