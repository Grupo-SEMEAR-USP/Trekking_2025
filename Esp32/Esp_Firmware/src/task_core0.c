#include "task_core0.h"
#include "initializers.h"
#include "sycronization.h"
#include "global_variables.h"
#include "pwm.h"
#include "pid.h"
#include "esp_timer.h"

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

int local_delta_encoder_ticks_left;
int local_delta_encoder_ticks_right;
    
int count_get_real;
int count_get_ros;

//Teste PID - speed from both wheels
int64_t current_time = 0;
int64_t last_time = 0;
int64_t delta_time = 0;

double ang_speed_left_wheel = 0, ang_speed_right_wheel = 0;

//variables for servo
float local_servo_angle;

float angle = 0, real_angle = 0;
int64_t t0 = 0, tf = 0, d_t = 0;

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
    ESP_ERROR_CHECK(encoder_left->set_glitch_filter(encoder_left, 10));
    ESP_ERROR_CHECK(encoder_right->set_glitch_filter(encoder_right, 10));

    // Start encoder
    ESP_ERROR_CHECK(encoder_left->start(encoder_left));
    ESP_ERROR_CHECK(encoder_right->start(encoder_right));
    
    //initializing pid

    pid_result_duty_left = 0.0;
    pid_result_duty_right = 0.0;

    local_ros_angular_speed_left = 0.0;
    local_ros_angular_speed_right = 0.0;

    local_motor_angular_speed_left = 0.0; 
    local_motor_angular_speed_right = 0.0;

    pid_init(&pid_handle_left,&pid_handle_right);
    
    //initializing servo
    local_servo_angle = (float) SERVO_INITIAL_ANGLE;

    //initializing variables for encoder and pid loop
    local_delta_encoder_ticks_left = 0;
    local_delta_encoder_ticks_right = 0;

    count_get_real = 0;
    count_get_ros = 0;

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


void monitor_encoder_pid_calc(void *params){

    if(count_get_real == ENCODER_COUNTER_WAIT_PID_OP){
            xSemaphoreTake(xSemaphore_getSpeed,portMAX_DELAY);
            //global_motor_angular_speed_left = ((float) encoder_left->get_counter_value(encoder_left))*(ENCODER_RESOLUTION/((float)count));
            //global_motor_angular_speed_right = ((float) encoder_right->get_counter_value(encoder_right))*(ENCODER_RESOLUTION/((float)count));
            
            local_delta_encoder_ticks_left = -encoder_left->get_counter_value(encoder_left);
            local_delta_encoder_ticks_right = encoder_right->get_counter_value(encoder_right);

            //printf("left: %d, right: %d\n", local_delta_encoder_ticks_left, local_delta_encoder_ticks_right);

            //reseting encoder stored ticks
            encoder_left->reset_counter_value(encoder_left);
            encoder_right->reset_counter_value(encoder_right);

            local_motor_angular_speed_left = (((float) local_delta_encoder_ticks_left)*ENCODER_RESOLUTION);
            local_motor_angular_speed_right = (((float) local_delta_encoder_ticks_right)*ENCODER_RESOLUTION);

            current_time = esp_timer_get_time();
            delta_time = current_time-last_time;
            last_time = current_time;
            ang_speed_left_wheel = (((local_delta_encoder_ticks_left*2*PI)/ENCODER_RESOLUTION_TICKS)/(delta_time))*1000000;
            ang_speed_right_wheel = (((local_delta_encoder_ticks_right*2*PI)/ENCODER_RESOLUTION_TICKS)/(delta_time))*1000000;
            
            //x and y displacements are given in milimeters
            global_total_x +=  (double)((local_delta_encoder_ticks_right+local_delta_encoder_ticks_left)*ENCODER_DISPLACEMENT*0.5*cos(global_total_theta));
            global_total_y +=  (double)((local_delta_encoder_ticks_right+local_delta_encoder_ticks_left)*ENCODER_DISPLACEMENT*0.5*sin(global_total_theta));
            printf("Setpoint(L): %lf / Real(L): %lf / Setpoint(R): %lf / Real(R): %lf\n", 
                local_ros_angular_speed_left,
                ang_speed_left_wheel,
                local_ros_angular_speed_right,
                ang_speed_right_wheel
            );
            global_total_theta += (double)(((local_delta_encoder_ticks_right-local_delta_encoder_ticks_left)*0.5*ENCODER_DISPLACEMENT)/WHELL_REAR_SEPARATION);//*ANGULAR_DISPLACEMENT);

            //printf("global_total_x: %lf, global_total_y: %lf, global_total_theta: %lf\n",global_total_x,global_total_y,global_total_theta);
            //global_servo_angle = local_servo_angle;

            global_time_stamp_miliseconds  = global_timer_miliseconds;

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
        
        //Checking if the speed sent from ROS is 0. If it is, it must nullifie the integral error
        if (fabsf(local_ros_angular_speed_left) < 1e-3f) {
            (pid_handle_left.pid_handle)->integral_err = 0.0f;
        }
        if (fabsf(local_ros_angular_speed_right) < 1e-3f) {
            (pid_handle_right.pid_handle)->integral_err = 0.0f;
        }

        pid_handle_left.pid_calculate(
            &pid_handle_left,
            ang_speed_left_wheel/*local_motor_angular_speed_left*/,
            local_ros_angular_speed_left,
            &pid_result_duty_left);

        pid_handle_right.pid_calculate(
            &pid_handle_right,
            ang_speed_right_wheel/*local_motor_angular_speed_right*/,
            local_ros_angular_speed_right,
            &pid_result_duty_right);
     
        //printf("duty_left: %lf, duty_right:%lf\n", pid_result_duty_left, pid_result_duty_right);
        pwm_actuate(ESQ,pid_result_duty_left);
        pwm_actuate(DIR,pid_result_duty_right);
        //pwm_actuate(ESQ,8191.0*(7.5/34));
        //pwm_actuate(DIR,8191.0*(22.0/37));
        iot_servo_write_angle(LEDC_LOW_SPEED_MODE, SERVO_PWM_CHANNEL, local_servo_angle);
        
        //printf("pwm_left: %f, pwm_right: %f / local_left: %f, local_right: %f\n", pid_result_duty_left, pid_result_duty_right, local_motor_angular_speed_left, local_motor_angular_speed_right);

        /*
        xSemaphoreTake(xSemaphore_getRosSpeed,portMAX_DELAY);
        printf("Deslocamento total em X: %f e em Y: %f \n", global_total_x, global_total_y);
        xSemaphoreGive(xSemaphore_getRosSpeed);
        //vTaskDelay(pdMS_TO_TICKS(PID_DELAY));
        */
        count_get_real++;
        count_get_ros++;

        global_timer_miliseconds += PID_DELAY;

}

static void IRAM_ATTR monitor_encoder_pid_calc_start(void *args)
{   
    //reseting encoder stored ticks
    encoder_left->reset_counter_value(encoder_left);
    encoder_right->reset_counter_value(encoder_right);

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

    //setting stop warning to hardware interface
    global_time_stamp_miliseconds = 0x7fffffff;
    //global_total_encoder_ticks_left = 0x7fffffff;
    //global_total_encoder_ticks_right = 0x7fffffff;

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
    gpio_set_intr_type(START_MOTOR_INTERRUPT_PIN, GPIO_INTR_POSEDGE);
    gpio_isr_handler_add(START_MOTOR_INTERRUPT_PIN, monitor_encoder_pid_calc_start, (void *)START_MOTOR_INTERRUPT_PIN);

    //install interrupt for stop pin
    gpio_set_intr_type(STOP_MOTOR_INTERRUPT_PIN, GPIO_INTR_POSEDGE);
    gpio_isr_handler_add(STOP_MOTOR_INTERRUPT_PIN, monitor_encoder_pid_calc_stop, (void *)STOP_MOTOR_INTERRUPT_PIN);

}