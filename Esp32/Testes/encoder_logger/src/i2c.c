#include "i2c.h"
#include "sycronization.h"
#include "global_variables.h"

void i2c_write(uint8_t *tx_data,float *angular_speed_left,float *angular_speed_right){
    xSemaphoreTake(xSemaphore_getSpeed,portMAX_DELAY);
    memcpy(tx_data,angular_speed_left,4);
    memcpy(tx_data+4,angular_speed_right,4);
    xSemaphoreGive(xSemaphore_getSpeed);

    i2c_slave_write_buffer(I2C_PORT,tx_data,MENSAGE_SIZE,portMAX_DELAY);

}

void i2c_read(uint8_t *rx_data,float *angular_speed_left,float *angular_speed_right){
    xSemaphoreTake(xSemaphore_getRosSpeed,portMAX_DELAY);
    if(i2c_slave_read_buffer(I2C_PORT,rx_data,MENSAGE_SIZE,portMAX_DELAY) > 0){
    
        memcpy(angular_speed_left, rx_data,4);
        memcpy(angular_speed_right, rx_data+4,4);
    }
    xSemaphoreGive(xSemaphore_getRosSpeed);
}