#pragma once
#include "config_params.hpp"

i2c_config_t i2c_config_info(int bsc_port,uint32_t slc_frequency,bool pull_up_en_sda, bool pull_up_en_slc);