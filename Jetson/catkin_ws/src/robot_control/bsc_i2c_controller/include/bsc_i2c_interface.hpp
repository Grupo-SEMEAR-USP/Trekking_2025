#pragma once 
#include "types.hpp"

// Pin configuration
#define GPFSEL0 (0/sizeof(uint32_t))
// Set output value
#define GPSET0 (0x1c/sizeof(uint32_t))
// Clear output
#define GPCLR0 (0x28/sizeof(uint32_t))
// Check level
#define GPLEV0 (0x34/sizeof(uint32_t))
// Pullup/Pulldown configuration bcm 2711
#define GPIO_PUP_PDN_CNTRL_REG0 (0xe4/sizeof(uint32_t))
// Pullup/Pulldown configuration bcm 2837 - 2835
#define GPPUD (0x94/sizeof(uint32_t))
#define GPPUDCLK0 (0x98/sizeof(uint32_t))

//i2c
#define I2C_C (0/sizeof(uint32_t))
// Status register
#define I2C_S (0x04/sizeof(uint32_t))
// Packet Length register
#define I2C_DLEN (0x08/sizeof(uint32_t))
// Address register
#define I2C_A (0x0c/sizeof(uint32_t))
// 8-bit wide data fifo register
#define I2C_FIFO (0x10/sizeof(uint32_t))

#define I2C_DIV (0x14/sizeof(uint32_t))

#define I2C_DEL (0x18/sizeof(uint32_t))

#define I2C_CLKT (0x1c/sizeof(uint32_t))

enum interrupt_mode{
    INTERRUPT_ON_DONE_BIT = 0x00000100, //bitwise or
    INTERRUPT_ON_TX_BIT = 0x00000200, //bitwise or
    INTERRUPT_ON_RX_BIT = 0x00000400, //bitwise or
    DISABLE_INTERRUPT = 0xfffff8ff //bitwise and
};

enum pull_up_down_mode{
    GPIO_PULLUP_ENABLE = 1,
    GPIO_PULLUP_DISABLE = 0
};

struct i2c_config_t{

    int i2c_port = 1; //BSC device index to be selected

    uint32_t slc_frequency = 100000; //Serial clock frequency in hertz

    //Time in microseconds the controller will wait a transfer process to finish 
    int64_t wait_port_time = 0;

    bool sda_pullup_en = GPIO_PULLUP_ENABLE; //Enables gpio pullup resistor on sda pin

    bool scl_pullup_en = GPIO_PULLUP_ENABLE; //Enables gpio pullup resistor on slc pin
    /**
    *Time in nano seconds to wait after the falling edge
    *of SCL before outputting next bit of data
    */
    double falling_edge_delay = 320;
    /**
    *Time in nano seconds to wait after the rising edge
    *of SCL before reading next bit of data
    */
    double rising_edge_delay = 320;
    /** 
    *Time in microseconds o wait after releasing SCL high and finding 
    *that the SCL is still low before deciding that the slave is not responding  
    */
    double wait_slave_timeout = 640; 

    uint32_t interrupt_mode = DISABLE_INTERRUPT;//Interrupts are not yet implemented so let the interrupt mode disabled

    bool use_10bit_adress = false;//Set BSC controller to use 10 bit adress, not yet implemented, let it false
};

struct i2c_slave_device_config_t{

    size_t tx_buffer_size = 8;//Send buffer size in bytes

    size_t rx_buffer_size = 8;//Recieve buffer size in bytes

    uint8_t i2c_slave_adress; //Slave adress, 7 bits only

};

struct i2c_slave_device_t{

    size_t tx_buffer_size = 8;//Send buffer size in bytes

    size_t rx_buffer_size = 8;//Recieve buffer size in bytes

    uint8_t i2c_slave_adress; //Slave adress, 7 bits only

    uint8_t *tx_buffer = nullptr;

    uint8_t *rx_buffer = nullptr;

};

class bsc_i2c_handle{

public:
    bsc_i2c_handle();
    ~bsc_i2c_handle();
    //bsc_i2c_handle();
    bsc_i2c_handle(const bsc_i2c_handle &) = delete;
    bsc_i2c_handle &operator=(const bsc_i2c_handle &) = delete;

    void i2c_param_config(i2c_config_t &config);
    void i2c_change_param_config(i2c_config_t &config);
    void set_slave_adress(std::string device_name, uint8_t adress);

    void i2c_start_bsc();
    void i2c_stop_bsc();
    void i2c_resume_bsc();
    void reset_fifo();
    void reset_done();

    void i2c_block();

    void set_write();
    void set_read();

    bool i2c_blocking_write(std::string device_name);
    bool i2c_blocking_read(std::string device_name);

    bool i2c_generic_write(std::string device_name, uint8_t* send_buffer, uint32_t send_buffer_size);
    bool i2c_generic_read(std::string device_name, uint8_t* recieve_buffer, uint32_t recieve_buffer_size);

    bool i2c_generic_non_blocking_write(std::string device_name, uint8_t* send_buffer, uint32_t send_buffer_size);
    bool i2c_generic_non_blocking_read(std::string device_name, uint8_t* recieve_buffer, uint32_t recieve_buffer_size);
    bool i2c_generic_read_from_bsc_fifo(uint8_t* recieve_buffer, uint32_t recieve_buffer_size);

    bool i2c_read(std::string device_name);
    bool i2c_write(std::string device_name);

    void i2c_add_slave_device(std::string device_name, i2c_slave_device_config_t &slave_device_config);

    //void i2c_read_from_adress();
    //int i2c_write_to_adress();

    /** 
    *Return the last transfer operation set
    *@return  1 if read and 0 if write
    */
    uint8_t get_last_transfer_op();
    /** 
    *Return the bsc unit status 
    *@return  0 if there is no operation set, 1 if an operation is done,
    2 if an operation is still happening
    */
    uint8_t get_bsc_unit_status();
    void wait_bsc_transfer(); //Wait until the bsc unit is freed

    friend void bcm_2711_init_gpio(bsc_i2c_handle* bsc_handle);
    friend void bcm_2837_2835_init_gpio(bsc_i2c_handle* bsc_handle);

    friend void bcm_2711_reset_gpio(bsc_i2c_handle* bsc_handle);
    friend void bcm_2837_2835_reset_gpio(bsc_i2c_handle* bsc_handle);

    std::map<std::string,i2c_slave_device_t> slave_devices;


private:
    std::vector<off_t> bsc_adresses;
    off_t gpio_base_adress;
    std::vector<std::vector<uint32_t>> gpio_map;
    uint32_t gpio_pin_sda;
    uint32_t gpio_pin_slc;
    uint32_t gpio_pinFunc_sda;
    uint32_t gpio_pinFunc_slc;

    bool gpio_sda_pull_up;
    bool gpio_slc_pull_up;
    //uint8_t slave_adress;

    int bsc_port;

    uint32_t core_clock;
    double core_clock_period;
    double slc_period;
    volatile uint32_t* bsc_unit;
    volatile uint32_t* gpio_base;
    int64_t port_delay_time;

    std::string last_slave_device = "no_device";

    void mmap_gpio();
    void mmap_i2c_bsc();

    void (*init_gpio)(bsc_i2c_handle* bsc_handle);
    void (*reset_gpio)(bsc_i2c_handle* bsc_handle);


    //size_t tx_buffer_size;
    //size_t rx_buffer_size;
    
};