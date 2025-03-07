#pragma once
#include "pid_ctrl.h"


typedef enum{
    DEFAULT_IMPLEMENTATION,
    GAP_DECREASED,
    CHICAO_METHOD
} pid_operation_mode;

typedef struct{
    pid_ctrl_config_t pid_config_info;
    pid_operation_mode operation_mode;
    float max_inertia_duty_value;
    float min_inertia_duty_value;
    float pos_gap_reduction;
    float neg_gap_reduction;

} pid_ctrl_set_operation;

typedef struct pid_handle_v2 pid_handle_v2;

struct pid_handle_v2{
    pid_ctrl_set_operation pid_set_operation_config;
    pid_ctrl_block_handle_t pid_handle;
    esp_err_t (*pid_calculate)(pid_handle_v2 *pid_handle_2, float real_value, float desired_value, float *result);
    esp_err_t (*delete_pid_handle)(pid_handle_v2 *pid_handle_2);
};



esp_err_t set_pid_operation(pid_handle_v2 *pid_v2);