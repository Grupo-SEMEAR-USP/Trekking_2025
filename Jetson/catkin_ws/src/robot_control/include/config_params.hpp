#pragma once
#include <bsc_i2c_interface.hpp>
#include <bsc_i2c_interface.hpp>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <chrono>
#include <cmath>

#define I2C_SLC_FREQUENCY 400000
#define BSC_UNIT_0 0
#define BSC_UNIT_1 1

#define I2C_SLAVE_ADRESS 0x69
#define I2C_TX_BUFFER_SIZE 12
#define I2C_RX_BUFFER_SIZE 16

//ros params
#define CMD_VEL_TIMEOUT_TIME 0.5//0.1
#define CMD_VEL_TIMEOUT_DEACELERATION_PERIOD 0.05

//hw params
#define HW_IF_UPDATE_FREQ 50//100
#define HW_IF_TICK_PERIOD 1 / HW_IF_UPDATE_FREQ

//inverse ackermann
