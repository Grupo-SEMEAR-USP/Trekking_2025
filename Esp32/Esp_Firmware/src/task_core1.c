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

    
    esp_err_t err = i2c_param_config(I2C_PORT, &i2c_slave_config);

    if (err != ESP_OK){
        printf("Erro na configuração do I2C!");
    }

    ESP_ERROR_CHECK(i2c_driver_install(I2C_PORT,I2C_MODE_SLAVE,I2C_SLAVE_RX_BUF_LEN,I2C_SLAVE_TX_BUF_LEN,0));

    uint8_t *rx_data = (uint8_t*) malloc(RX_MENSAGE_SIZE);
    uint8_t *tx_data = (uint8_t*) malloc(TX_MENSAGE_SIZE);

    xEventGroupSetBits(initialization_groupEvent, task1_init_done);

    while(true){

        //printf("Dados enviados: %f, %f, %f, %d\n", global_total_x, global_total_y, global_total_theta, global_time_stamp_miliseconds);
        i2c_write(tx_data, &global_total_x, &global_total_y, &global_total_theta, &global_time_stamp_miliseconds);
        i2c_read(rx_data, &global_ros_angular_speed_left, &global_ros_angular_speed_right, &global_ros_servo_angle);
        vTaskDelay(pdMS_TO_TICKS(I2C_DELAY));
    }

}