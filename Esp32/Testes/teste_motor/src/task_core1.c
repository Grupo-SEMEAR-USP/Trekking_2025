#include "task_core1.h"
#include "initializers.h"
#include "sycronization.h"
#include "global_variables.h"
#include "i2c.h"

void core1functions(void *params){

    xEventGroupWaitBits(initialization_groupEvent, task0_init_done, true, true, portMAX_DELAY);

    i2c_config_t i2c_slave_config = i2c_slave_config_info(
        I2C_SLAVE_SDA_IO,
        I2C_SLAVE_SCL_IO,
        (uint16_t) ESP_SLAVE_ADDR);

    i2c_param_config(I2C_PORT, &i2c_slave_config);

    i2c_driver_install(I2C_PORT,I2C_MODE_SLAVE,I2C_SLAVE_RX_BUF_LEN,I2C_SLAVE_TX_BUF_LEN,0);

    uint8_t *rx_data = (uint8_t*) malloc(MENSAGE_SIZE);
    uint8_t *tx_data = (uint8_t*) malloc(MENSAGE_SIZE);
    
   
    xEventGroupSetBits(initialization_groupEvent, task1_init_done);
    /*/comente
    xSemaphoreTake(xSemaphore_getRosSpeed,portMAX_DELAY);    
    global_ros_angular_speed_left=50.0;
    global_ros_angular_speed_right=50.0;
    xSemaphoreGive(xSemaphore_getRosSpeed);
    */

    while(true){

        //i2c_write(tx_data, &global_motor_angular_speed_left, &global_motor_angular_speed_right);

        //i2c_read(rx_data, &global_ros_angular_speed_left, &global_ros_angular_speed_right);

        xSemaphoreTake(xSemaphore_getRosSpeed,portMAX_DELAY);    
        global_ros_angular_speed_left=20.0;
        global_ros_angular_speed_right=20.0;
        xSemaphoreGive(xSemaphore_getRosSpeed);

        vTaskDelay(pdMS_TO_TICKS(I2C_DELAY));
    }

}