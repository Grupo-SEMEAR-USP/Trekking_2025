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

    int local_delta_encoder_ticks_left;
    int local_delta_encoder_ticks_right;
    
    int count_get_real;
    int count_get_ros;
    int count_sample;


//timer function to read encoder and calculate pid
void monitor_encoder_pid_calc(void *params);

//create timer handle
TimerHandle_t monitor_encoder_pid_calc_timer_handle;
int sample_finished = 0;

#define Ziger_time 8000
#define N_samples (Ziger_time/PID_DELAY)

double displacement_vector_left[N_samples];
double displacement_vector_right[N_samples];

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

    //reseting encoder stored ticks
    encoder_left->reset_counter_value(encoder_left);
    encoder_right->reset_counter_value(encoder_right);
    
    //initializing pid

    pid_result_duty_left = 0;
    pid_result_duty_right = 0;

    local_ros_angular_speed_left = 0;
    local_ros_angular_speed_right = 0;

    local_motor_angular_speed_left = 0; 
    local_motor_angular_speed_right = 0;

    //pid_init(&pid_handle_left,&pid_handle_right);
    
    count_get_real = 0;
    count_get_ros = 0;
    count_sample = 0;

    //xEventGroupSetBits(initialization_groupEvent, task0_init_done);
    //xEventGroupWaitBits(initialization_groupEvent, task1_init_done, true, true, portMAX_DELAY);
    
    //create timer for reading encoder and calculating pid
    monitor_encoder_pid_calc_timer_handle = xTimerCreate("Timer do encoder e pid",pdMS_TO_TICKS(PID_DELAY),pdTRUE,NULL,monitor_encoder_pid_calc);

    vTaskDelay(pdMS_TO_TICKS(3000));

    //reseting encoder stored ticks
    encoder_left->reset_counter_value(encoder_left);
    encoder_right->reset_counter_value(encoder_right);

    //init timer
    pwm_actuate(ESQ,2000);
    pwm_actuate(DIR,2000);
    xTimerStart(
    monitor_encoder_pid_calc_timer_handle 
    ,0);
    //vTaskSuspend(NULL);
/*
    while(true){
        if(sample_finished == 1){
            for(int i = 0; i >= N_samples; i++){
                printf("%f,%f,%d",displacement_vector_left[i],displacement_vector_left[i],i*PID_DELAY*ENCODER_COUNTER_WAIT_PID_OP);
            }
            break;
        }
        else{
            printf("Oi");
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
*/
    //vTaskDelay(pdMS_TO_TICKS(9999999999));
    vTaskSuspend(NULL);


}

void monitor_encoder_pid_calc(void *params){

    //if(count_get_real == ENCODER_COUNTER_WAIT_PID_OP){

            //xSemaphoreTake(xSemaphore_getSpeed,portMAX_DELAY);
            local_delta_encoder_ticks_left = encoder_left->get_counter_value(encoder_left);
            local_delta_encoder_ticks_right = encoder_right->get_counter_value(encoder_right);
            
            //x and y displacements are given in milimeters
            displacement_vector_left[count_sample] =  (double)((local_delta_encoder_ticks_left)*ENCODER_DISPLACEMENT);
            displacement_vector_right[count_sample]=  (double)((local_delta_encoder_ticks_right)*ENCODER_DISPLACEMENT);
            //xSemaphoreGive(xSemaphore_getSpeed);
            
            printf("%f,%f,%d\n",displacement_vector_left[count_sample],displacement_vector_right[count_sample],count_sample*PID_DELAY);
            count_sample++;

            //count_get_real = 0;

            if(count_sample >= N_samples){
                
                //vTaskResume(Core0_Task_Handle);
                pwm_actuate(ESQ,0);
                pwm_actuate(DIR,0);
                //sample_finished =1;
                xTimerStop(
                monitor_encoder_pid_calc_timer_handle 
                ,0);



            }
        //}

        //count_get_real++;
        //count_get_ros++;

}

