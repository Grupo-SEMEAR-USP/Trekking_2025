#include <bsc_i2c_interface.hpp>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <chrono>
#include <cmath>

int main(){

    i2c_config_t i2c_config = {};
        i2c_config.i2c_port = 1; 
        i2c_config.slc_frequency = 400000; 
        i2c_config.wait_port_time = 200;
        i2c_config.sda_pullup_en = GPIO_PULLUP_ENABLE; 
        i2c_config.scl_pullup_en = GPIO_PULLUP_ENABLE; 
        i2c_config.falling_edge_delay = 320;
        i2c_config.rising_edge_delay = 320;
        i2c_config.wait_slave_timeout = 640; 
        i2c_config.interrupt_mode = DISABLE_INTERRUPT;
        i2c_config.use_10bit_adress = false;

    i2c_slave_device_config_t slave_config;
        slave_config.tx_buffer_size = 8;
        slave_config.rx_buffer_size = 8;
        slave_config.i2c_slave_adress = 0x69;

    bsc_i2c_handle i2c_master;
    i2c_master.i2c_param_config(i2c_config);
    i2c_master.i2c_add_slave_device("espFeliz", slave_config);
    i2c_master.i2c_start_bsc();

    uint8_t val[] = {1,2,3,4,5,6,7,8};
    uint8_t val2[8] = {0,0,0,0,0,0,0,0};
    
    usleep(2000000);

    float left=0;
    float right=0;
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;
    while (1){
        memcpy(i2c_master.slave_devices["espFeliz"].tx_buffer,val,8);
        start  = std::chrono::high_resolution_clock::now();
        i2c_master.i2c_blocking_write("espFeliz");
        end  = std::chrono::high_resolution_clock::now();
        
        std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "[µs]" << std::endl;
        usleep(1500);
        start  = std::chrono::high_resolution_clock::now();
        if(i2c_master.i2c_blocking_read("espFeliz")){
        end  = std::chrono::high_resolution_clock::now();
        std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "[µs]" << std::endl;
        memcpy(val2, i2c_master.slave_devices["espFeliz"].rx_buffer,8);
        memcpy(&left, val2,4);
        memcpy(&right, val2+4,4);
        
        //for (int i = 0; i < 8 ;i++){
        //    std::cout<<(int)i2c_master.slave_devices["espFeliz"].rx_buffer[i];
        //}
        //std::cout<<std::endl;
        std::cout<<"left: "<<left<<" right: "<<right;
        std::cout<<std::endl;
        }
        
        usleep(1500);
        
    }

    return 0;
}