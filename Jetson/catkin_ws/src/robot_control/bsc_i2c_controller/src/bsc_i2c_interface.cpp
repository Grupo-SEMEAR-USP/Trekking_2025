#include "GetRPiModel.h"
#include "bsc_i2c_interface.hpp"
#include <math.h>   

void bcm_2711_init_gpio(bsc_i2c_handle* bsc_handle);
void bcm_2837_2835_init_gpio(bsc_i2c_handle* bsc_handle);
void bcm_2711_reset_gpio(bsc_i2c_handle* bsc_handle);
void bcm_2837_2835_reset_gpio(bsc_i2c_handle* bsc_handle);

bsc_i2c_handle::bsc_i2c_handle(){
    GetRPiModel *model = new GetRPiModel();
    switch (model->socket()){
		case SocketModel::BCM2711:
			bsc_adresses = {0xfe205000,0xfe804000,0xfe205600,0xfe205800,0xfe205a80,0xfe205c00};
			gpio_base_adress = 0xfe200000;
			gpio_map = {{0,1,0b100,0b100},{2,3,0b100,0b100},{4,5,0b010,0b010},{8,9,0b010,0b010},{10,11,0b010,0b010},{22,23,0b010,0b010}};
			init_gpio = bcm_2711_init_gpio;
			reset_gpio = bcm_2711_reset_gpio;
			break;
        case SocketModel::BCM2837:
			bsc_adresses = {0x3F205000,0x3F804000};
			gpio_base_adress = 0x3F200000;
			gpio_map = {{0,1,0b100,0b100},{2,3,0b100,0b100}};
			init_gpio = bcm_2837_2835_init_gpio;
			reset_gpio = bcm_2837_2835_reset_gpio; 
			break;
        case SocketModel::BCM2837B0:
			bsc_adresses = {0x3F205000,0x3F804000};
			gpio_base_adress = 0x3F200000; 
			gpio_map = {{0,1,0b100,0b100},{2,3,0b100,0b100}}; 
			init_gpio = bcm_2837_2835_init_gpio;
			reset_gpio = bcm_2837_2835_reset_gpio;   
			break;
		case SocketModel::BCM2835:
			bsc_adresses = {0x20205000,0x20804000};
			gpio_base_adress = 0x20200000; 
			gpio_map = {{0,1,0b100,0b100},{2,3,0b100,0b100}};
			init_gpio = bcm_2837_2835_init_gpio;
			reset_gpio = bcm_2837_2835_reset_gpio;    
			break;
		default:
			throw std::runtime_error("device not yet supported!");
			break;
	}
    delete model;

	std::ifstream cpu_clock("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq", std::ios::in);
	if (!cpu_clock.is_open()) {
        throw std::runtime_error("failed to get cpu clock!");
    }
	std::string line;
	std::getline(cpu_clock, line);
	core_clock = (uint32_t) stoi(line);
	core_clock *= (uint32_t) 1000;
	core_clock_period = ((double)1e9)/((double)core_clock);



}

bsc_i2c_handle::~bsc_i2c_handle(){

	bsc_unit[I2C_C] &= 0xffff78ce; //deactivate bsc,reset clear,set write,reset interrupt bits

	bsc_unit[I2C_S] |= 0x302;//reset DONE,ERR,CLKT

	reset_fifo();

	//clear gpio

	reset_gpio(this);


}

void bsc_i2c_handle::i2c_add_slave_device(std::string device_name, i2c_slave_device_config_t &slave_device_config){
	if(slave_device_config.i2c_slave_adress > 0x7f) throw std::runtime_error("Slave adress must be 7 bit size!");

	i2c_slave_device_t slave_device = {
		.tx_buffer_size = slave_device_config.tx_buffer_size,
		.rx_buffer_size = slave_device_config.rx_buffer_size,
		.i2c_slave_adress = slave_device_config.i2c_slave_adress,
		.tx_buffer = nullptr,
		.rx_buffer = nullptr
	};

	slave_devices.insert(std::pair<std::string,i2c_slave_device_t>(device_name,slave_device));
	slave_devices[device_name].rx_buffer = (uint8_t*) malloc(slave_device.rx_buffer_size);
	slave_devices[device_name].tx_buffer = (uint8_t*) malloc(slave_device.tx_buffer_size);
}

void bsc_i2c_handle::i2c_param_config(i2c_config_t &config){

	port_delay_time = config.wait_port_time;

	bsc_port = config.i2c_port;

	gpio_pin_sda = gpio_map[bsc_port][0];
    gpio_pin_slc = gpio_map[bsc_port][1];
    gpio_pinFunc_sda = gpio_map[bsc_port][2];
    gpio_pinFunc_slc = gpio_map[bsc_port][3];

	gpio_sda_pull_up = config.sda_pullup_en;
	gpio_slc_pull_up = config.scl_pullup_en;

	mmap_gpio();
    mmap_i2c_bsc();

	init_gpio(this);

	bsc_unit[I2C_C] &= 0xffff78ce; //deactivate bsc,reset clear,set write,reset interrupt bits

	bsc_unit[I2C_S] |= 0x302;//reset DONE,ERR,CLKT

	reset_fifo();

	if(config.interrupt_mode == DISABLE_INTERRUPT){
		bsc_unit[I2C_C] &= DISABLE_INTERRUPT;
	}
	else{
		bsc_unit[I2C_C] |= config.interrupt_mode;
	}

	uint32_t div_value = (uint32_t)(core_clock/config.slc_frequency);

	bsc_unit[I2C_DIV] &= 0xffff0000;
	bsc_unit[I2C_DIV] |= div_value;
	
	//setting glich filter
	uint32_t rising_edge_delay_count = (uint32_t) round((config.rising_edge_delay/core_clock_period));
	uint32_t falling_edge_delay_count = (uint32_t) round((config.falling_edge_delay/core_clock_period));

	bsc_unit[I2C_DEL] = (falling_edge_delay_count << 16) | rising_edge_delay_count; //falling edge,asing edge

	slc_period = ((double)1e6)/((double)config.slc_frequency);
	bsc_unit[I2C_CLKT] &= 0xffff0000;
	bsc_unit[I2C_CLKT] |= (uint32_t) round((config.wait_slave_timeout/slc_period));

}

void bsc_i2c_handle::set_slave_adress(std::string device_name, uint8_t adress){
	if(adress > 0x7f) throw std::runtime_error("Slave adress must be 7 bit size!");
	slave_devices[device_name].i2c_slave_adress = adress;
	//bsc_unit8[BSC_BYTE_OFFSET*3] &= (uint8_t) 0b10000000; //reset adress
	//bsc_unit8[BSC_BYTE_OFFSET*3] |= adress;
}

void bsc_i2c_handle::i2c_start_bsc(){
	bsc_unit[I2C_C] |= 0x00008000; 
	reset_fifo();
}

void bsc_i2c_handle::i2c_resume_bsc(){
	bsc_unit[I2C_C] |= 0x00008000; 
}

void bsc_i2c_handle::i2c_stop_bsc(){
	bsc_unit[I2C_C] &= 0xffff7fff;  //deactivate bsc
}

void bsc_i2c_handle::reset_fifo()
{
  // Clear FIFO
  bsc_unit[I2C_C] |= 0x00000030;
}

void bsc_i2c_handle::reset_done()
{
  bsc_unit[I2C_S] |= 1 << 1;
}

void bsc_i2c_handle::i2c_block()
{
  while (((bsc_unit[I2C_S] & 0x00000002) >> 1) != 1) {
	std::this_thread::sleep_for(std::chrono::microseconds(10));
  }
}

void bsc_i2c_handle::set_write()
{
  bsc_unit[I2C_C] &= 0xfffffffe;//set write
}

void bsc_i2c_handle::set_read()
{
  bsc_unit[I2C_C] |= 0x00000001;//set read
}

void bsc_i2c_handle::i2c_change_param_config(i2c_config_t &config){
	i2c_param_config(config);
	i2c_start_bsc();
}

bool bsc_i2c_handle::i2c_read(std::string device_name){
	//verify CLKT, ERR, TA, WR, DONE
	uint8_t status = ((bsc_unit[I2C_S] & 0x00000300) << 3) | //get CLKT, ERR
		((bsc_unit[I2C_S] & 0x00000001) << 2)| //get TA
		((bsc_unit[I2C_C] & 0x00000001) << 1) | //get WR
		((bsc_unit[I2C_S] & 0x00000002) >> 1); //get DONE;
	
	if(status < 8){
		if(status & 0b00000100){
			std::this_thread::sleep_for(std::chrono::microseconds(port_delay_time));
			if((bsc_unit[I2C_S] & 0x00000001)) return 0;
		}

		//*************read algorithm**************
		//if(!(status & 0b00000001)) reset_fifo();//reset fifo
		if(!(status & 0b00000010)){ //if write

			bsc_unit[I2C_A] &= 0xffffff80;
			bsc_unit[I2C_A] |= (uint32_t) slave_devices[device_name].i2c_slave_adress;

			bsc_unit[I2C_DLEN] &= 0xffff0000;
			bsc_unit[I2C_DLEN] |= (uint32_t) slave_devices[device_name].rx_buffer_size;
			bsc_unit[I2C_C] |= 0x00000001;//set read
		
			reset_fifo();
			reset_done();//reset DONE
			bsc_unit[0] |= 0x00000080; //start

		} else{
			
			for(size_t i = 0; i < slave_devices[last_slave_device].rx_buffer_size; i++){
				slave_devices[last_slave_device].rx_buffer[i] = (uint8_t) (bsc_unit[I2C_FIFO] & 0xff);
			}

			bsc_unit[I2C_A] &= 0xffffff80;
			bsc_unit[I2C_A] |= (uint32_t) slave_devices[device_name].i2c_slave_adress;

			bsc_unit[I2C_DLEN] &= 0xffff0000;
			bsc_unit[I2C_DLEN] |= (uint32_t) slave_devices[device_name].rx_buffer_size;
			bsc_unit[I2C_C] |= 0x00000001;//set read
		
			reset_fifo();
			reset_done();//reset DONE
			bsc_unit[0] |= 0x00000080; //start

		}
		//*************************************************

	}
	else{

		bsc_unit[I2C_S] |= 0x00000300; //reset CLKT, ERR
		return false;
	}

	last_slave_device = device_name;

	return true;
}

bool bsc_i2c_handle::i2c_write(std::string device_name){
	//verify CLKT, ERR, TA, WR, DONE
	uint8_t status = ((bsc_unit[I2C_S] & 0x00000300) << 3) | //get CLKT, ERR
		((bsc_unit[I2C_S] & 0x00000001) << 2)| //get TA
		((bsc_unit[I2C_C] & 0x00000001) << 1) | //get WR
		((bsc_unit[I2C_S] & 0x00000002) >> 1); //get DONE;
	
	if(status < 8){
		if(status & 0b00000100){
			std::this_thread::sleep_for(std::chrono::microseconds(port_delay_time));
			if((bsc_unit[I2C_S] & 0x00000001)) return 0;
		}

		//*************write algorithm**************
		//if(!(status & 0b00000001)) reset_fifo();//reset fifo
		if(!(status & 0b00000010)){ //if write
			
			bsc_unit[I2C_A] &= 0xffffff80;
			bsc_unit[I2C_A] |= (uint32_t) slave_devices[device_name].i2c_slave_adress;

			bsc_unit[I2C_DLEN] &= 0xffff0000;
			bsc_unit[I2C_DLEN] |= (uint32_t) slave_devices[device_name].tx_buffer_size;
			bsc_unit[I2C_C] &= 0xfffffffe;//set write
			reset_fifo();
			for(size_t i = 0; i < slave_devices[device_name].tx_buffer_size; i++){
				bsc_unit[I2C_FIFO] = (uint32_t) slave_devices[device_name].tx_buffer[i];
			}
			reset_done();//reset DONE
			bsc_unit[0] |= 0x00000080; //start

		} else{
			
			for(size_t i = 0; i < slave_devices[last_slave_device].rx_buffer_size; i++){
				slave_devices[last_slave_device].rx_buffer[i] = (uint8_t) (bsc_unit[I2C_FIFO] & 0xff);
			}

			bsc_unit[I2C_A] &= 0xffffff80;
			bsc_unit[I2C_A] |= (uint32_t) slave_devices[device_name].i2c_slave_adress;

			bsc_unit[I2C_DLEN] &= 0xffff0000;
			bsc_unit[I2C_DLEN] |= (uint32_t) slave_devices[device_name].tx_buffer_size;
			bsc_unit[I2C_C] &= 0xfffffffe;//set write
			reset_fifo();
			for(size_t i = 0; i < slave_devices[device_name].tx_buffer_size; i++){
				bsc_unit[I2C_FIFO] = (uint32_t) slave_devices[device_name].tx_buffer[i];
			}
			reset_done();//reset DONE
			bsc_unit[0] |= 0x00000080; //start

		}
		//*************************************************

	}
	else{

		bsc_unit[I2C_S] |= 0x00000300; //reset CLKT, ERR
		return false;
	}

	last_slave_device = device_name;

	return true;
	
}

bool bsc_i2c_handle::i2c_blocking_write(std::string device_name){
	uint8_t status = ((bsc_unit[I2C_S] & 0x00000300) << 3) | //get CLKT, ERR
		((bsc_unit[I2C_S] & 0x00000001) << 2)| //get TA
		((bsc_unit[I2C_C] & 0x00000001) << 1) | //get WR
		((bsc_unit[I2C_S] & 0x00000002) >> 1); //get DONE;

	if(status < 8){
		//*************write algorithm**************
		//if(!(status & 0b00000001)) reset_fifo();//reset fifo
		
		bsc_unit[I2C_A] &= 0xffffff80;
		bsc_unit[I2C_A] |= (uint32_t) slave_devices[device_name].i2c_slave_adress;
		bsc_unit[I2C_DLEN] &= 0xffff0000;
		bsc_unit[I2C_DLEN] |= (uint32_t) slave_devices[device_name].tx_buffer_size;
		bsc_unit[I2C_C] &= 0xfffffffe;//set write
		reset_fifo();
		for(size_t i = 0; i < slave_devices[device_name].tx_buffer_size; i++){
			bsc_unit[I2C_FIFO] = (uint32_t) slave_devices[device_name].tx_buffer[i];
		}
		reset_done();//reset DONE
		bsc_unit[0] |= 0x00000080; //start

		i2c_block();
		//*************************************************

	}
	else{

		bsc_unit[I2C_S] |= 0x00000300; //reset CLKT, ERR
		return false;
	}

	return true;
	
	
}

bool bsc_i2c_handle::i2c_blocking_read(std::string device_name){

	uint8_t status = ((bsc_unit[I2C_S] & 0x00000300) << 3) | //get CLKT, ERR
		((bsc_unit[I2C_S] & 0x00000001) << 2)| //get TA
		((bsc_unit[I2C_C] & 0x00000001) << 1) | //get WR
		((bsc_unit[I2C_S] & 0x00000002) >> 1); //get DONE;

	if(status < 8){
		//*************read algorithm**************
		//if(!(status & 0b00000001)) reset_fifo();//reset fifo
		
		bsc_unit[I2C_A] &= 0xffffff80;
		bsc_unit[I2C_A] |= (uint32_t) slave_devices[device_name].i2c_slave_adress;
		bsc_unit[I2C_DLEN] &= 0xffff0000;
		bsc_unit[I2C_DLEN] |= (uint32_t) slave_devices[device_name].rx_buffer_size;
		bsc_unit[I2C_C] |= 0x00000001;//set read
		reset_fifo();
		reset_done();//reset DONE
		bsc_unit[0] |= 0x00000080; //start

		i2c_block();

		for(size_t i = 0; i < slave_devices[device_name].rx_buffer_size; i++){
				slave_devices[device_name].rx_buffer[i] = (uint8_t) (bsc_unit[I2C_FIFO] & 0xff);
			}
		//*************************************************

	}
	else{

		bsc_unit[I2C_S] |= 0x00000300; //reset CLKT, ERR
		return false;
	}

	return true;
	

}

bool bsc_i2c_handle::i2c_generic_write(std::string device_name, uint8_t* send_buffer, uint32_t send_buffer_size){

	uint8_t status = ((bsc_unit[I2C_S] & 0x00000300) << 3) | //get CLKT, ERR
		((bsc_unit[I2C_S] & 0x00000001) << 2)| //get TA
		((bsc_unit[I2C_C] & 0x00000001) << 1) | //get WR
		((bsc_unit[I2C_S] & 0x00000002) >> 1); //get DONE;

	if(status < 8){

		//*************write algorithm**************
		//if(!(status & 0b00000001)) reset_fifo();//reset fifo
		
		bsc_unit[I2C_A] &= 0xffffff80;
		bsc_unit[I2C_A] |= (uint32_t) slave_devices[device_name].i2c_slave_adress;
		bsc_unit[I2C_DLEN] &= 0xffff0000;
		bsc_unit[I2C_DLEN] |= (uint32_t) send_buffer_size;
		bsc_unit[I2C_C] &= 0xfffffffe;//set write
		reset_fifo();
		for(uint32_t i = 0; i < send_buffer_size; i++){
			bsc_unit[I2C_FIFO] = (uint32_t) send_buffer[i];
		}
		reset_done();//reset DONE
		bsc_unit[0] |= 0x00000080; //start

		i2c_block();
		//*************************************************
		

	}
	else{

		bsc_unit[I2C_S] |= 0x00000300; //reset CLKT, ERR
		return false;
	}

	return true;

}

bool bsc_i2c_handle::i2c_generic_read(std::string device_name, uint8_t* recieve_buffer, uint32_t recieve_buffer_size){

	uint8_t status = ((bsc_unit[I2C_S] & 0x00000300) << 3) | //get CLKT, ERR
		((bsc_unit[I2C_S] & 0x00000001) << 2)| //get TA
		((bsc_unit[I2C_C] & 0x00000001) << 1) | //get WR
		((bsc_unit[I2C_S] & 0x00000002) >> 1); //get DONE;

	if(status < 8){
		//*************read algorithm**************
		//if(!(status & 0b00000001)) reset_fifo();//reset fifo
		
		bsc_unit[I2C_A] &= 0xffffff80;
		bsc_unit[I2C_A] |= (uint32_t) slave_devices[device_name].i2c_slave_adress;
		bsc_unit[I2C_DLEN] &= 0xffff0000;
		bsc_unit[I2C_DLEN] |= (uint32_t) recieve_buffer_size;
		bsc_unit[I2C_C] |= 0x00000001;//set read
		reset_fifo();
		reset_done();//reset DONE
		bsc_unit[0] |= 0x00000080; //start

		i2c_block();

		for(uint32_t i = 0; i < recieve_buffer_size; i++){
				recieve_buffer[i] = (uint8_t) (bsc_unit[I2C_FIFO] & 0xff);
			}
		//*************************************************

	}
	else{

		bsc_unit[I2C_S] |= 0x00000300; //reset CLKT, ERR
		return false;
	}

	return true;
}

bool bsc_i2c_handle::i2c_generic_non_blocking_write(std::string device_name, uint8_t* send_buffer, uint32_t send_buffer_size){

	uint8_t status = ((bsc_unit[I2C_S] & 0x00000300) << 3) | //get CLKT, ERR
		((bsc_unit[I2C_S] & 0x00000001) << 2)| //get TA
		((bsc_unit[I2C_C] & 0x00000001) << 1) | //get WR
		((bsc_unit[I2C_S] & 0x00000002) >> 1); //get DONE;

	if(status < 8){

		//*************write algorithm**************
		//if(!(status & 0b00000001)) reset_fifo();//reset fifo
		
		bsc_unit[I2C_A] &= 0xffffff80;
		bsc_unit[I2C_A] |= (uint32_t) slave_devices[device_name].i2c_slave_adress;
		bsc_unit[I2C_DLEN] &= 0xffff0000;
		bsc_unit[I2C_DLEN] |= (uint32_t) send_buffer_size;
		bsc_unit[I2C_C] &= 0xfffffffe;//set write
		reset_fifo();
		for(uint32_t i = 0; i < send_buffer_size; i++){
			bsc_unit[I2C_FIFO] = (uint32_t) send_buffer[i];
		}
		reset_done();//reset DONE
		bsc_unit[0] |= 0x00000080; //start

		//*************************************************
		

	}
	else{

		bsc_unit[I2C_S] |= 0x00000300; //reset CLKT, ERR
		return false;
	}

	return true;

}

bool bsc_i2c_handle::i2c_generic_non_blocking_read(std::string device_name, uint8_t* recieve_buffer, uint32_t recieve_buffer_size){

	uint8_t status = ((bsc_unit[I2C_S] & 0x00000300) << 3) | //get CLKT, ERR
		((bsc_unit[I2C_S] & 0x00000001) << 2)| //get TA
		((bsc_unit[I2C_C] & 0x00000001) << 1) | //get WR
		((bsc_unit[I2C_S] & 0x00000002) >> 1); //get DONE;

	if(status < 8){
		//*************read algorithm**************
		//if(!(status & 0b00000001)) reset_fifo();//reset fifo
		
		bsc_unit[I2C_A] &= 0xffffff80;
		bsc_unit[I2C_A] |= (uint32_t) slave_devices[device_name].i2c_slave_adress;
		bsc_unit[I2C_DLEN] &= 0xffff0000;
		bsc_unit[I2C_DLEN] |= (uint32_t) recieve_buffer_size;
		bsc_unit[I2C_C] |= 0x00000001;//set read
		reset_fifo();
		reset_done();//reset DONE
		bsc_unit[0] |= 0x00000080; //start

		//*************************************************

	}
	else{

		bsc_unit[I2C_S] |= 0x00000300; //reset CLKT, ERR
		return false;
	}

	return true;
}

bool bsc_i2c_handle::i2c_generic_read_from_bsc_fifo(uint8_t* recieve_buffer, uint32_t recieve_buffer_size){

	uint8_t status = ((bsc_unit[I2C_S] & 0x00000300) << 3) | //get CLKT, ERR
		((bsc_unit[I2C_S] & 0x00000001) << 2)| //get TA
		((bsc_unit[I2C_C] & 0x00000001) << 1) | //get WR
		((bsc_unit[I2C_S] & 0x00000002) >> 1); //get DONE;

	if(status < 8){

		for(uint32_t i = 0; i < recieve_buffer_size; i++){
				recieve_buffer[i] = (uint8_t) (bsc_unit[I2C_FIFO] & 0xff);
			}
	}
	else{

		bsc_unit[I2C_S] |= 0x00000300; //reset CLKT, ERR
		return false;
	}

	return true;
}


uint8_t bsc_i2c_handle::get_last_transfer_op(){
	return (bsc_unit[I2C_C] & 0x00000001); 
}

uint8_t bsc_i2c_handle::get_bsc_unit_status(){
	return (((bsc_unit[I2C_S] & 0x00000001) << 1)| //get TA
			((bsc_unit[I2C_S] & 0x00000002) >> 1)); //get DONE
}

void bsc_i2c_handle::wait_bsc_transfer(){
	i2c_block();
}

void bsc_i2c_handle::mmap_gpio()
{
	int gpio_base_fd = open("/dev/mem", O_RDWR | O_SYNC);

	if (gpio_base_fd == -1) {		
		throw std::runtime_error("error opening /dev/mem for gpio controller");
	}	
		gpio_base = (volatile uint32_t *)mmap((void *)gpio_base_adress, 0x100, PROT_READ | PROT_WRITE, MAP_SHARED, gpio_base_fd, gpio_base_adress);

	if (gpio_base == (void *)-1) {
		throw std::runtime_error("error opening /dev/mem for gpio controller");
	}
		
}

//bsc_unit = (volatile uint32_t*) bsc_adresses.at(config.i2c_port);

//gpio_base = (volatile uint32_t*) gpio_base_adress;

void bsc_i2c_handle::mmap_i2c_bsc()
{
	int i2c_bsc_base_fd = open("/dev/mem", O_RDWR | O_SYNC);

	if (i2c_bsc_base_fd == -1) {		
		throw std::runtime_error("error opening /dev/mem for i2c bsc controller");
	}

	bsc_unit = (volatile uint32_t *)mmap((void *)bsc_adresses.at(bsc_port), 0x20, PROT_READ | PROT_WRITE, MAP_SHARED, i2c_bsc_base_fd, bsc_adresses.at(bsc_port));
	
	if (bsc_unit == (void *)-1) {
		throw std::runtime_error("error opening /dev/mem for i2c bsc controller");		
	}

}

void bcm_2711_init_gpio(bsc_i2c_handle* bsc_handle)
{
	// Clear config for GPIO 2 and 3 (pin 3 and 5)
	uint32_t gpfsel_index_sda = bsc_handle->gpio_pin_sda/10;
	uint32_t gpfsel_bitpos_sda = bsc_handle->gpio_pin_sda%10;
	uint32_t gpfsel_index_slc = bsc_handle->gpio_pin_slc/10;
	uint32_t gpfsel_bitpos_slc = bsc_handle->gpio_pin_slc%10;

	uint32_t gpio_pup_pdn_index_sda = bsc_handle->gpio_pin_sda/16;
	uint32_t gpio_pup_pdn_bitpos_sda = bsc_handle->gpio_pin_sda%16;
	uint32_t gpio_pup_pdn_index_slc = bsc_handle->gpio_pin_slc/16;
	uint32_t gpio_pup_pdn_bitpos_slc = bsc_handle->gpio_pin_slc%16;


	//gpio_base[GPFSEL0] &= ~0x00000FC0;
	bsc_handle->gpio_base[gpfsel_index_sda] &= ~(7 << (gpfsel_bitpos_sda*3));
	bsc_handle->gpio_base[gpfsel_index_slc] &= ~(7 << (gpfsel_bitpos_slc*3));
	// Set GPIO 2 and 3 to Alt Func 0: SDA1/SDC1 (pin 3 and 5)
	//gpio_base[GPFSEL0] |= 0x00000900;
	bsc_handle->gpio_base[gpfsel_index_sda] |= (bsc_handle->gpio_pinFunc_sda << (gpfsel_bitpos_sda*3));
	bsc_handle->gpio_base[gpfsel_index_slc] |= (bsc_handle->gpio_pinFunc_slc << (gpfsel_bitpos_slc*3));

	// Clear GPIO 2 and 3 resistor config.
	//gpio_base[GPIO_PUP_PDN_CNTRL_REG0] &= ~0x000000F0;
	bsc_handle->gpio_base[GPIO_PUP_PDN_CNTRL_REG0 + gpio_pup_pdn_index_sda] &= ~(3 << (gpio_pup_pdn_bitpos_sda*2));
	bsc_handle->gpio_base[GPIO_PUP_PDN_CNTRL_REG0 + gpio_pup_pdn_index_slc] &= ~(3 << (gpio_pup_pdn_bitpos_slc*2));
	// Set GPIO 2 and 3 pullup resistor config.
	//gpio_base[GPIO_PUP_PDN_CNTRL_REG0] |= 0x00000050;
	if(bsc_handle->gpio_sda_pull_up){bsc_handle->gpio_base[GPIO_PUP_PDN_CNTRL_REG0 + gpio_pup_pdn_index_sda] |= (1 << (gpio_pup_pdn_bitpos_sda*2));}
	if(bsc_handle->gpio_slc_pull_up){bsc_handle->gpio_base[GPIO_PUP_PDN_CNTRL_REG0 + gpio_pup_pdn_index_slc] |= (1 << (gpio_pup_pdn_bitpos_slc*2));}
	
 	//printf("GPFSEL0 (0x%08x) = 0x%08x\n", &gpioctrl[GPFSEL0], gpioctrl[GPFSEL0]);
 	//printf("PUP_PDN (0x%08x) = 0x%08x\n", &gpioctrl[GPIO_PUP_PDN_CNTRL_REG0], gpioctrl[GPIO_PUP_PDN_CNTRL_REG0]);
 	//printf("GPLEV0 (0x%08x) = 0x%08x\n", &gpioctrl[GPLEV0], gpioctrl[GPLEV0]);
}

void bcm_2837_2835_init_gpio(bsc_i2c_handle* bsc_handle)
{
	// Clear config for GPIO 2 and 3 (pin 3 and 5)
	uint32_t gpfsel_index_sda = bsc_handle->gpio_pin_sda/10;
	uint32_t gpfsel_bitpos_sda = bsc_handle->gpio_pin_sda%10;
	uint32_t gpfsel_index_slc = bsc_handle->gpio_pin_slc/10;
	uint32_t gpfsel_bitpos_slc = bsc_handle->gpio_pin_slc%10;

	uint32_t gpio_pup_pdn_index_sda = bsc_handle->gpio_pin_sda/32;
	uint32_t gpio_pup_pdn_bitpos_sda = bsc_handle->gpio_pin_sda%32;
	uint32_t gpio_pup_pdn_index_slc = bsc_handle->gpio_pin_slc/32;
	uint32_t gpio_pup_pdn_bitpos_slc = bsc_handle->gpio_pin_slc%32;


	//gpio_base[GPFSEL0] &= ~0x00000FC0;
	bsc_handle->gpio_base[gpfsel_index_sda] &= ~(7 << (gpfsel_bitpos_sda*3));
	bsc_handle->gpio_base[gpfsel_index_slc] &= ~(7 << (gpfsel_bitpos_slc*3));
	// Set GPIO 2 and 3 to Alt Func 0: SDA1/SDC1 (pin 3 and 5)
	//gpio_base[GPFSEL0] |= 0x00000900;
	bsc_handle->gpio_base[gpfsel_index_sda] |= (bsc_handle->gpio_pinFunc_sda << (gpfsel_bitpos_sda*3));
	bsc_handle->gpio_base[gpfsel_index_slc] |= (bsc_handle->gpio_pinFunc_slc << (gpfsel_bitpos_slc*3));

	bsc_handle->gpio_base[GPPUD] = 0x0; //deactivate pullup function for all gpio
	std::this_thread::sleep_for(std::chrono::milliseconds(5));
	// Clear GPIO 2 and 3 resistor config.
	//gpio_base[GPIO_PUP_PDN_CNTRL_REG0] &= ~0x000000F0;
	bsc_handle->gpio_base[GPPUDCLK0 + gpio_pup_pdn_index_sda] = (1 << gpio_pup_pdn_bitpos_sda);
	bsc_handle->gpio_base[GPPUDCLK0 + gpio_pup_pdn_index_slc] = (1 << gpio_pup_pdn_bitpos_slc);
	// Set GPIO 2 and 3 pullup resistor config.
	//gpio_base[GPIO_PUP_PDN_CNTRL_REG0] |= 0x00000050;
	bsc_handle->gpio_base[GPPUD] = 0x2; //activate pullup function for all gpio
	std::this_thread::sleep_for(std::chrono::milliseconds(5));
	if(bsc_handle->gpio_sda_pull_up){bsc_handle->gpio_base[GPPUDCLK0 + gpio_pup_pdn_index_sda] = (1 << gpio_pup_pdn_bitpos_sda);}
	if(bsc_handle->gpio_slc_pull_up){bsc_handle->gpio_base[GPPUDCLK0 + gpio_pup_pdn_index_slc] = (1 << gpio_pup_pdn_bitpos_slc);}
	
 	//printf("GPFSEL0 (0x%08x) = 0x%08x\n", &gpioctrl[GPFSEL0], gpioctrl[GPFSEL0]);
 	//printf("PUP_PDN (0x%08x) = 0x%08x\n", &gpioctrl[GPIO_PUP_PDN_CNTRL_REG0], gpioctrl[GPIO_PUP_PDN_CNTRL_REG0]);
 	//printf("GPLEV0 (0x%08x) = 0x%08x\n", &gpioctrl[GPLEV0], gpioctrl[GPLEV0]);
}

void bcm_2711_reset_gpio(bsc_i2c_handle* bsc_handle){

	uint32_t gpfsel_index_sda = bsc_handle->gpio_pin_sda/10;
	uint32_t gpfsel_bitpos_sda = bsc_handle->gpio_pin_sda%10;
	uint32_t gpfsel_index_slc = bsc_handle->gpio_pin_slc/10;
	uint32_t gpfsel_bitpos_slc = bsc_handle->gpio_pin_slc%10;

	uint32_t gpio_pup_pdn_index_sda = bsc_handle->gpio_pin_sda/16;
	uint32_t gpio_pup_pdn_bitpos_sda = bsc_handle->gpio_pin_sda%16;
	uint32_t gpio_pup_pdn_index_slc = bsc_handle->gpio_pin_slc/16;
	uint32_t gpio_pup_pdn_bitpos_slc = bsc_handle->gpio_pin_slc%16;

	bsc_handle->gpio_base[gpfsel_index_sda] &= ~(7 << (gpfsel_bitpos_sda*3));
	bsc_handle->gpio_base[gpfsel_index_slc] &= ~(7 << (gpfsel_bitpos_slc*3));

	bsc_handle->gpio_base[GPIO_PUP_PDN_CNTRL_REG0 + gpio_pup_pdn_index_sda] &= ~(3 << (gpio_pup_pdn_bitpos_sda*2));
	bsc_handle->gpio_base[GPIO_PUP_PDN_CNTRL_REG0 + gpio_pup_pdn_index_slc] &= ~(3 << (gpio_pup_pdn_bitpos_slc*2));
}

void bcm_2837_2835_reset_gpio(bsc_i2c_handle* bsc_handle){

	// Clear config for GPIO 2 and 3 (pin 3 and 5)
	uint32_t gpfsel_index_sda = bsc_handle->gpio_pin_sda/10;
	uint32_t gpfsel_bitpos_sda = bsc_handle->gpio_pin_sda%10;
	uint32_t gpfsel_index_slc = bsc_handle->gpio_pin_slc/10;
	uint32_t gpfsel_bitpos_slc = bsc_handle->gpio_pin_slc%10;

	uint32_t gpio_pup_pdn_index_sda = bsc_handle->gpio_pin_sda/32;
	uint32_t gpio_pup_pdn_bitpos_sda = bsc_handle->gpio_pin_sda%32;
	uint32_t gpio_pup_pdn_index_slc = bsc_handle->gpio_pin_slc/32;
	uint32_t gpio_pup_pdn_bitpos_slc = bsc_handle->gpio_pin_slc%32;


	//gpio_base[GPFSEL0] &= ~0x00000FC0;
	bsc_handle->gpio_base[gpfsel_index_sda] &= ~(7 << (gpfsel_bitpos_sda*3));
	bsc_handle->gpio_base[gpfsel_index_slc] &= ~(7 << (gpfsel_bitpos_slc*3));

	std::this_thread::sleep_for(std::chrono::milliseconds(5));
	bsc_handle->gpio_base[GPPUD] = 0x0; //deactivate pullup function for all gpio
	std::this_thread::sleep_for(std::chrono::milliseconds(5));

	bsc_handle->gpio_base[GPPUDCLK0 + gpio_pup_pdn_index_sda] = (1 << gpio_pup_pdn_bitpos_sda);
	bsc_handle->gpio_base[GPPUDCLK0 + gpio_pup_pdn_index_slc] = (1 << gpio_pup_pdn_bitpos_slc);
}