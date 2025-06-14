#pragma once

// Incluir esta lib antes de timers.h
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "driver/ledc.h"
#include "freertos/event_groups.h"
#include "rotary_encoder.h"
#include "pid_ctrl.h"
#include "pid_ctrl_v2.h"
#include "driver/i2c.h"
#include "iot_servo.h"
#include <string.h>

//start and stop interrupts gpio constants
#define START_MOTOR_INTERRUPT_PIN GPIO_NUM_35
#define STOP_MOTOR_INTERRUPT_PIN GPIO_NUM_34


//pwm gpio constants
#define ENABLE_A GPIO_NUM_32
#define INPUT_1 GPIO_NUM_33
#define INPUT_2 GPIO_NUM_18

#define ENABLE_ESQ ENABLE_A

#define ENABLE_B GPIO_NUM_4
#define INPUT_3 GPIO_NUM_19
#define INPUT_4 GPIO_NUM_25

#define ENABLE_DIR ENABLE_B

//servo pwm constants

#define SERVO_DUTY_PIN 13
#define SERVO_PWM_TIMER LEDC_TIMER_2//pwm channel for servo
#define SERVO_PWM_CHANNEL LEDC_CHANNEL_2
#define SERVO_INITIAL_ANGLE 180.0
#define SERVO_OFFSET 0

//hardware definitions for pwm 
#define PWM_FREQ 2000
#define MAX_DUTY 8191

//logical definitions to help choosing the right motor
#define ESQ 0 //input 1, input 2 e enable A
#define DIR 1 //input 3, input 4 e enable B

//Macros to help identificating which motor will be used
#define channel_choose(channel) ((channel == ESQ) ? LEDC_CHANNEL_0 : LEDC_CHANNEL_1) //Canal do timer do PWM
#define timer_choose(timer) ((timer == ESQ) ? LEDC_TIMER_0 : LEDC_TIMER_1) //Timer escolhido
#define enable_choose(channel) ((channel == ESQ) ? ENABLE_ESQ : ENABLE_DIR) //Pino do enable
#define motor_choose_in_primary(motor) ((motor == ESQ) ? INPUT_1 : INPUT_3) //Pino "primário" do motor
#define motor_choose_in_secundary(motor) ((motor == ESQ) ? INPUT_2 : INPUT_4) //Pino "secundário" do motor

//i2c constants
#define I2C_DELAY 4
#define TIMEOUT_MS_WRITE 0.001
#define TIMEOUT_MS_READ 0.001
#define I2C_PORT 0
#define MENSAGE_SIZE 8

#define I2C_SLAVE_SCL_IO GPIO_NUM_22 //gpio number for i2c slave clock 
#define I2C_SLAVE_SDA_IO GPIO_NUM_21 //gpio number for i2c slave data
#define ESP_SLAVE_ADDR 0x69 //0x69 == ESP que controla a traseira, 0x68 == ESP que controla a frente

#define I2C_SLAVE_TX_BUF_LEN 256
#define I2C_SLAVE_RX_BUF_LEN 256

//pid constants
#define PID_DELAY 8

#define MAX_PID_VALUE 8191.0
#define MAX_INTEGRAL_VALUE 3880.0

// Kp,Ki,Kd constants
#define KP_LEFT 1.0
#define KI_LEFT 1.0
#define KD_LEFT 1.0

#define KP_RIGHT 1.0
#define KI_RIGHT 1.0
#define KD_RIGHT 1.0


//limits of pwm duty cycle values to escape inertia
#define MAX_INERTIA_DUTY_LEFT 1500
#define MIN_INERTIA_DUTY_LEFT -1500

#define MAX_INERTIA_DUTY_RIGHT 1500
#define MIN_INERTIA_DUTY_RIGHT -1500

//limits of pwm duty cycle values to create a secure
//gap area which will be discarted on pid computations
#define POS_GAP_REDUCTION_LEFT 1400
#define NEG_GAP_REDUCTION_LEFT -1400

#define POS_GAP_REDUCTION_RIGHT 1400
#define NEG_GAP_REDUCTION_RIGHT -1400

//encoder contasnts
#define PCNT_CHA_LEFT 14
#define PCNT_CHB_LEFT 5

#define PCNT_CHA_RIGHT 26
#define PCNT_CHB_RIGHT 27

#define PI 3.14159

#define ENCODER_RESOLUTION_TICKS 1320
#define ENCODER_COUNTER_WAIT_PID_OP  4  //number of pid operations required to extrac encoder speed
#define GET_ROS_VAL_COUNTER_WAIT_PID_OP  1
#define ENCODER_RESOLUTION (((2*PI)/(ENCODER_RESOLUTION_TICKS))/(PID_DELAY*ENCODER_COUNTER_WAIT_PID_OP))*1000 
