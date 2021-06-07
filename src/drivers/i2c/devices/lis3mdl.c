#include <drivers/i2c/lis3mdl.h>
#include <stdint.h>
#include <utils.h>
#include <stdio.h>

#define LIS3MDL_DEVICE_ADDRESS      0x1E
#define LIS3MDL_DEVICE_ID           0x3D

#define REG_WHO_AM_I               0x0F
#define REG_CTRL_REG1              0x20
#define REG_CTRL_REG2              0x21
#define REG_CTRL_REG3              0x22
#define REG_CTRL_REG4              0x23
#define REG_CTRL_REG5              0x24

#define REG_STATUS_REG             0x27
#define REG_OUT_X_L                0x28
#define REG_OUT_X_H                0x29
#define REG_OUT_Y_L                0x2A
#define REG_OUT_Y_H                0x2B
#define REG_OUT_Z_L                0x2C
#define REG_OUT_Z_H                0x2D


static inline uint8_t LIS3MDL_get_device_id(i2c_bus_driver *driver);
static uint8_t LIS3MDL_read_register(uint8_t reg, i2c_bus_driver *driver);
static uint8_t LIS3MDL_write_register(uint8_t reg,uint8_t data, i2c_bus_driver *driver);

int LIS3MDL_create_device(i2c_bus_driver *i2c_drv, LIS3MDL_driver_t *drv)
{
    int ret = 0;
    int device_id = 0;

    device_id = LIS3MDL_get_device_id(i2c_drv);
    if (device_id == LIS3MDL_DEVICE_ID) {
        drv->driver = i2c_drv;
        LIS3MDL_run_device(drv);
    } else {
        ret = -1;
    }

    return ret;
}
int16_t LIS3MDL_get_magnetic_field_X(LIS3MDL_driver_t *drv)
{
    uint8_t data[2];

    data[0] = LIS3MDL_read_register(REG_OUT_X_L,drv->driver);
    data[1] = LIS3MDL_read_register(REG_OUT_X_H,drv->driver);

    return *((int16_t*)data);
}
int16_t LIS3MDL_get_magnetic_field_Y(LIS3MDL_driver_t *drv)
{
    uint8_t data[2];

    data[0] = LIS3MDL_read_register(REG_OUT_Y_L,drv->driver);
    data[1] = LIS3MDL_read_register(REG_OUT_Y_H,drv->driver);

    return *((int16_t*)data);
}
int16_t LIS3MDL_get_magnetic_field_Z(LIS3MDL_driver_t *drv)
{
    uint8_t data[2];

    data[0] = LIS3MDL_read_register(REG_OUT_Z_L,drv->driver);
    data[1] = LIS3MDL_read_register(REG_OUT_Z_H,drv->driver);

    return *((int16_t*)data);
}
static uint8_t LIS3MDL_get_device_id(i2c_bus_driver *driver)
{
    return LIS3MDL_read_register(REG_WHO_AM_I,driver);
}

static uint8_t LIS3MDL_read_register(uint8_t reg, i2c_bus_driver *driver)
{
    uint8_t readData = 0x00;
    uint8_t wrData = reg;
    i2c_transfer transfer;
    
    transfer.device_address = LIS3MDL_DEVICE_ADDRESS;
    transfer.read_data.data_ptr = &readData;
    transfer.read_data.data_size = 1;
    transfer.write_data.data_ptr = &wrData;
    transfer.write_data.data_size = 1;
    i2c_request_transfer(driver,&transfer);

    return readData;
}

static uint8_t LIS3MDL_write_register(uint8_t reg,uint8_t data, i2c_bus_driver *driver)
{
    uint8_t wrData[2];

    wrData[0] = reg;
    wrData[1] = data;

    i2c_transfer transfer;
    
    transfer.device_address = LIS3MDL_DEVICE_ADDRESS;
    transfer.read_data.data_ptr = NULL;
    transfer.read_data.data_size = 0;
    transfer.write_data.data_ptr = wrData;
    transfer.write_data.data_size = 2;
    i2c_request_transfer(driver,&transfer);

    return 0;
}

void LIS3MDL_run_device(LIS3MDL_driver_t *drv)
{
    // correct it to look nicer
    LIS3MDL_write_register(REG_CTRL_REG3, 0,drv->driver);
}
