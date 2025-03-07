#pragma once
#include "types.h"

void i2c_write(uint8_t *tx_data,double *total_x_displacement,double *total_y_displacement,double *total_angular_displacement,uint32_t *time_stamp);
void i2c_read(uint8_t *rx_data,float *angular_speed_left,float *angular_speed_right,float *servo_angle);