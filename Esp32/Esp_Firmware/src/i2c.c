#include "i2c.h"
#include "sycronization.h"
#include "global_variables.h"

void i2c_write(uint8_t *tx_data,double *total_x_displacement,double *total_y_displacement,double *total_angular_displacement,uint32_t *time_stamp){
    
    xSemaphoreTake(xSemaphore_getSpeed,portMAX_DELAY);
    
    //variables for storing displacement in micrometers to send
    static int total_x_micrometers;
    static int total_y_micrometers;
    static int total_theta_mmrad;
    
    total_x_micrometers = (int)((*total_x_displacement)*1000);
    total_y_micrometers = (int)((*total_y_displacement)*1000);
    total_theta_mmrad = (int)((*total_angular_displacement)*1000);

    memcpy(tx_data,&total_x_micrometers,4);
    memcpy(tx_data+4,&total_y_micrometers,4);
    memcpy(tx_data+8,&total_theta_mmrad,4);
    memcpy(tx_data+12,time_stamp,4);

    xSemaphoreGive(xSemaphore_getSpeed);

    //printf("global_total_x: %d, global_total_y: %d, global_total_theta: %d,time: %u\n",total_x_micrometers,total_y_micrometers,total_theta_mmrad,*time_stamp);
    //printf("data: %d,%d,%d,%u\n",*(((int*)tx_data)),*(((int*)tx_data)+1),*(((int*)tx_data)+2),*(((int*)tx_data)+3));
    i2c_slave_write_buffer(I2C_PORT,tx_data,TX_MENSAGE_SIZE,TIMEOUT_MS_WRITE / portTICK_RATE_MS);
}

void i2c_read(uint8_t *rx_data,float *angular_speed_left,float *angular_speed_right,float *servo_angle){
    xSemaphoreTake(xSemaphore_getRosSpeed,portMAX_DELAY);
    
    if(i2c_slave_read_buffer(I2C_PORT,rx_data,RX_MENSAGE_SIZE,TIMEOUT_MS_READ / portTICK_RATE_MS) > 0){
        memcpy(angular_speed_left, rx_data,4);
        memcpy(angular_speed_right, rx_data+4,4);
        memcpy(servo_angle, rx_data+8,4);

        //printf("left: ");
        //printf("left: %f",*angular_speed_left);
        //printf("right: %f",*angular_speed_right);
        //printf("servo: %f\n",*servo_angle);
    }
    xSemaphoreGive(xSemaphore_getRosSpeed);
}