#ifndef __I2C_BUS_H__
#define __I2C_BUS_H__

#include <stdint.h>
#include <stdbool.h>

typedef enum i2c_operation_type
{
    I2C_WRITE,
    I2C_READ,
    I2C_READ_AFTER_WRITE
} i2c_operation;

typedef enum i2c_transfer_mode_type
{
    I2C_BLOCKING,
    I2C_NONBLOCKING
} i2c_transfer_mode;

typedef enum i2c_speed_type 
{
    I2C_100kHz,
    I2C_400kHz
} i2c_speed;

typedef struct i2c_data_type
{
    uint8_t* data_ptr;
    uint8_t  data_size;
} i2c_data;

typedef struct i2c_transfer_type {
    uint8_t device_address;
    i2c_operation rw;
    i2c_data write_data;
    i2c_data read_data;
    bool transfer_ongoing;
    // on_transfer_finished is designed for nonblocking operations
    void (*on_transfer_finished)(struct i2c_transfer_type* transfer);
} i2c_transfer;

typedef struct i2c_bus_fun
{
    int (*initialize)(void * prv_data);
    int (*set_mode) (void* prv_data, i2c_transfer_mode mode);
    int (*transfer) (void *prv_data, i2c_transfer* transfer);
} i2c_bus_functions;

typedef struct i2c_bus_driver_type
{
    uint8_t device_name[16];
    i2c_bus_functions *functions;
    void* prv_data;
} i2c_bus_driver;

int i2c_initialize_bus(i2c_bus_driver* driver);
int i2c_set_transfer_mode(i2c_bus_driver* driver,i2c_transfer_mode);
int i2c_request_transfer(i2c_bus_driver* driver, i2c_transfer* transfer);

#endif