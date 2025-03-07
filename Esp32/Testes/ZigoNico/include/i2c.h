#pragma once
#include "types.h"

void i2c_write(uint8_t *tx_data,float *angular_speed_left,float *angular_speed_right);
void i2c_read(uint8_t *rx_data,float *angular_speed_left,float *angular_speed_right);