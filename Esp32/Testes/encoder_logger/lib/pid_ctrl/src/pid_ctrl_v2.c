#include "pid_ctrl_v2.h"
#include <stdbool.h>
#include <sys/param.h>
#include "esp_check.h"
#include "esp_log.h"

static const char *TAGR = "pid_ctrl_v2";

static esp_err_t default_compute(pid_handle_v2 *pid_handle_2, float real_value,float desired_value, float *result){

    float difference = desired_value - real_value;

    return pid_compute(pid_handle_2->pid_handle, difference, result);
}

static esp_err_t gap_decreased_compute(pid_handle_v2 *pid_handle_2, float real_value,float desired_value, float *result){

    float difference = desired_value - real_value;

    esp_err_t err = pid_compute(pid_handle_2->pid_handle, difference, result);

    (*result >= 0) ? (*result += pid_handle_2->pid_set_operation_config.pos_gap_reduction) : 
    (*result += pid_handle_2->pid_set_operation_config.neg_gap_reduction);

    return err;
}

static esp_err_t chicao_compute(pid_handle_v2 *pid_handle_2, float real_value,float desired_value, float *result){

    float difference = desired_value - real_value;

    esp_err_t err = pid_compute(pid_handle_2->pid_handle, difference,result);

    if (desired_value == 0)
        *result = 0;

    
    if (*result < 0.2*(pid_handle_2->pid_set_operation_config.min_inertia_duty_value)){
        if (*result > pid_handle_2->pid_set_operation_config.min_inertia_duty_value && *result != 0){ 
            *result = pid_handle_2->pid_set_operation_config.min_inertia_duty_value;
        }
        else if(*result < pid_handle_2->pid_set_operation_config.pid_config_info.init_param.min_output && *result!= 0){
            *result = pid_handle_2->pid_set_operation_config.pid_config_info.init_param.min_output;
        }
    }
    else if (*result > 0.2*(pid_handle_2->pid_set_operation_config.max_inertia_duty_value)){
        if (*result < pid_handle_2->pid_set_operation_config.max_inertia_duty_value && *result != 0){
            *result = pid_handle_2->pid_set_operation_config.max_inertia_duty_value;
        }
        else if(*result > pid_handle_2->pid_set_operation_config.pid_config_info.init_param.max_output && *result != 0){
            *result = pid_handle_2->pid_set_operation_config.pid_config_info.init_param.max_output;
        }
    }
    else{
        *result = 0;
    }

    return err;
    
}
 
static esp_err_t pid_delete(pid_handle_v2 *pid_handle_2){
    pid_del_control_block(pid_handle_2->pid_handle);
    free(pid_handle_2);
    return ESP_OK;
}


esp_err_t set_pid_operation( pid_handle_v2 *pid_v2){

    switch (pid_v2->pid_set_operation_config.operation_mode) {
    case DEFAULT_IMPLEMENTATION:
        pid_new_control_block(&(pid_v2->pid_set_operation_config.pid_config_info), &(pid_v2->pid_handle));
        pid_v2->pid_calculate = default_compute;
        break;
    case GAP_DECREASED:

        pid_v2->pid_set_operation_config.pid_config_info.init_param.max_output -= 
        pid_v2->pid_set_operation_config.pos_gap_reduction;

        pid_v2->pid_set_operation_config.pid_config_info.init_param.min_output -= 
        pid_v2->pid_set_operation_config.neg_gap_reduction;

        pid_new_control_block(&(pid_v2->pid_set_operation_config.pid_config_info), &(pid_v2->pid_handle));

        pid_v2->pid_calculate = gap_decreased_compute;
        break;
    case CHICAO_METHOD:
        pid_new_control_block(&(pid_v2->pid_set_operation_config.pid_config_info), &(pid_v2->pid_handle));
        pid_v2->pid_calculate = chicao_compute;
        break;
    default:
        ESP_RETURN_ON_FALSE(false, ESP_ERR_INVALID_ARG, TAGR, "invalid PID calculation type:%d", pid_v2->pid_set_operation_config.operation_mode);
    }
    pid_v2->delete_pid_handle = pid_delete;

    
    return ESP_OK;

}