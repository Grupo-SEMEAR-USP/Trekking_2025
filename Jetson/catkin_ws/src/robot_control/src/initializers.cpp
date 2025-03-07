#include "initializers.hpp"

i2c_config_t i2c_config_info(int bsc_port,uint32_t slc_frequency,bool pull_up_en_sda, bool pull_up_en_slc){

        i2c_config_t i2c_config = {};
        i2c_config.i2c_port = bsc_port; 
        i2c_config.slc_frequency = slc_frequency; 
        i2c_config.wait_port_time = 200;
        i2c_config.sda_pullup_en = pull_up_en_sda; 
        i2c_config.scl_pullup_en = pull_up_en_slc; 
        i2c_config.falling_edge_delay = 320;
        i2c_config.rising_edge_delay = 320;
        i2c_config.wait_slave_timeout = 640; 
        i2c_config.interrupt_mode = DISABLE_INTERRUPT;
        i2c_config.use_10bit_adress = false;

        return i2c_config;

}