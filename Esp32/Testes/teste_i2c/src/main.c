#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "driver/ledc.h"
#include "freertos/event_groups.h"
#include "driver/i2c.h"
#include <string.h>
#include <math.h>

#define I2C_DELAY 4
#define TIMEOUT_MS_WRITE 0.001
#define TIMEOUT_MS_READ 0.001
#define I2C_PORT 0
#define MENSAGE_SIZE 8

#define I2C_SLAVE_SCL_IO GPIO_NUM_22 //gpio number for i2c slave clock 
#define I2C_SLAVE_SDA_IO GPIO_NUM_21 //gpio number for i2c slave data
#define ESP_SLAVE_ADDR 0x58 //0x69 == ESP que controla a traseira, 0x68 == ESP que controla a frente

#define I2C_SLAVE_TX_BUF_LEN 256
#define I2C_SLAVE_RX_BUF_LEN 256



i2c_config_t i2c_slave_config_info(
    gpio_num_t sda_pin,
    gpio_num_t slc_pin,
    uint16_t slave_addr){

    i2c_config_t conf_slave;
        conf_slave.mode = I2C_MODE_SLAVE;
        conf_slave.sda_io_num = (int) sda_pin;
        conf_slave.scl_io_num = (int) slc_pin;          
        conf_slave.sda_pullup_en = GPIO_PULLUP_DISABLE;           
        conf_slave.scl_pullup_en = GPIO_PULLUP_DISABLE;
        conf_slave.slave.addr_10bit_en = 0;
        conf_slave.slave.slave_addr = slave_addr;       
        conf_slave.slave.maximum_speed = 400000;
        conf_slave.clk_flags = 0;

    return  conf_slave;                          
    
}

void app_main() {

    TickType_t tickstowait = pdMS_TO_TICKS(1);

    i2c_config_t i2c_slave_config = i2c_slave_config_info(
        I2C_SLAVE_SDA_IO,
        I2C_SLAVE_SCL_IO,
        (uint16_t) ESP_SLAVE_ADDR);

    i2c_param_config(I2C_PORT, &i2c_slave_config);

    i2c_driver_install(I2C_PORT,I2C_MODE_SLAVE,(size_t)I2C_SLAVE_RX_BUF_LEN,(size_t)I2C_SLAVE_TX_BUF_LEN,0);

    uint8_t *rx_data = (uint8_t*) malloc(MENSAGE_SIZE);
    uint8_t *tx_data = (uint8_t*) malloc(MENSAGE_SIZE);
    float right = 0;
    float left = 0;
    //uint8_t *nome = (uint8_t*)calloc(MENSAGE_SIZE,1);
    uint8_t *nome = (uint8_t*)malloc(MENSAGE_SIZE);
    //uint8_t nome2[8] = {1,2,3,4,5,6,7,8};

    float count = 1.0f;
    while(true){

        //int64_t curr_ts = esp_timer_get_time();
        if(count == 11.0f) {count = 1.0f;right = 0.0f;left = 0.0f;}

        if(i2c_slave_read_buffer(I2C_PORT,rx_data,(size_t)MENSAGE_SIZE,tickstowait/4) > 0 ){ //TIMEOUT_MS_READ / portTICK_RATE_MS
            memcpy(nome, rx_data,8);
            for(int i =0 ; i<8;i++){
                //printf("%d",nome[i]);
            }
            //printf("\n");
        }
    
        
        //7*sin() + 1;
        memcpy(tx_data,(void*) &left,4);
        memcpy(tx_data+4,(void*) &right,4);

        i2c_slave_write_buffer(I2C_PORT,tx_data,MENSAGE_SIZE,tickstowait/4);
    
        count += 1.0f;
        right += 1.0f;
        left -= 1.0f;
        vTaskDelay(pdMS_TO_TICKS(I2C_DELAY));
    }
    
}