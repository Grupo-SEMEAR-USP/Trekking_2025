#include "pid.h"
#include "initializers.h"

void pid_init(pid_handle_v2 *pid_handle_left,pid_handle_v2 *pid_handle_right){

    float ks_left[] = {KP_LEFT,KI_LEFT,KD_LEFT};
    float ks_right[] = {KP_RIGHT,KI_RIGHT,KD_RIGHT};

    pid_ctrl_set_operation set_operation_info_left = {
        .pid_config_info = pid_ctrl_config_info(ks_left,MAX_PID_VALUE, MAX_INTEGRAL_VALUE),
        .operation_mode = GAP_DECREASED,
        .max_inertia_duty_value = MAX_INERTIA_DUTY_LEFT,
        .min_inertia_duty_value = MIN_INERTIA_DUTY_LEFT,
        .pos_gap_reduction = POS_GAP_REDUCTION_LEFT,
        .neg_gap_reduction = NEG_GAP_REDUCTION_LEFT
    };

    pid_ctrl_set_operation set_operation_info_right = {
        .pid_config_info = pid_ctrl_config_info(ks_right,MAX_PID_VALUE, MAX_INTEGRAL_VALUE),
        .operation_mode = GAP_DECREASED,
        .max_inertia_duty_value = MAX_INERTIA_DUTY_RIGHT,
        .min_inertia_duty_value = MIN_INERTIA_DUTY_RIGHT,
        .pos_gap_reduction = POS_GAP_REDUCTION_RIGHT,
        .neg_gap_reduction = NEG_GAP_REDUCTION_RIGHT
    };

    pid_handle_left->pid_set_operation_config = set_operation_info_left;
    pid_handle_right->pid_set_operation_config = set_operation_info_right;

    set_pid_operation(pid_handle_left);
    set_pid_operation(pid_handle_right);
}
