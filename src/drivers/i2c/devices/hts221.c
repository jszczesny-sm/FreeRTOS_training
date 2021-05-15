#include <drivers/i2c/hts221.h>
#include <stdint.h>
#include <utils.h>
#include <stdio.h>

#define HTS221_DEVICE_ADDRESS      0x5F
#define HTS221_DEVICE_ID           0xBC

#define REG_WHO_AM_I               0x0F
#define REG_AV_CONF                0x10
#define REG_CTRL_REG1              0x20
#define REG_CTRL_REG2              0x21
#define REG_CTRL_REG3              0x22

#define REG_STATUS_REG             0x27
#define REG_HUMIDITY_LO            0x28
#define REG_HUMIDITY_HI            0x29
#define REG_TEMPERATURE_LO         0x2A
#define REG_TEMPERATURE_HI         0x2B

// Temperature calibration parameters
#define REG_T0_deg_C_x8            0x32
#define REG_T1_deg_C_x8            0x33
#define REG_T0T1_deg_MSB_x8        0x35

#define REG_T0_out_HI              0x3D
#define REG_T0_out_LO              0x3C
#define REG_T1_out_HI              0x3F
#define REG_T1_out_LO              0x3E


static inline uint8_t HTS221_get_device_id(i2c_bus_driver *driver);
static uint8_t HTS221_read_register(uint8_t reg, i2c_bus_driver *driver);
static uint8_t HTS221_write_register(uint8_t reg,uint8_t data, i2c_bus_driver *driver);

static inline void HTS221_read_calibration(HTS221_driver_t *drv);

int HTS221_create_device(i2c_bus_driver *i2c_drv, HTS221_driver_t *drv)
{
    int ret = 0;
    int device_id = 0;

    device_id = HTS221_get_device_id(i2c_drv);
    if (device_id == HTS221_DEVICE_ID) {
        drv->driver = i2c_drv;
        HTS221_read_calibration(drv);
    } else {
        ret = -1;
    }

    return ret;
}

int16_t HTS221_get_temperature(HTS221_driver_t *drv)
{
    uint8_t data[2];

    data[0] = HTS221_read_register(REG_TEMPERATURE_LO,drv->driver);
    data[1] = HTS221_read_register(REG_TEMPERATURE_HI,drv->driver);

    return *((int16_t*)data);
}

static uint8_t HTS221_get_device_id(i2c_bus_driver *driver)
{
    return HTS221_read_register(REG_WHO_AM_I,driver);
}

static uint8_t HTS221_read_register(uint8_t reg, i2c_bus_driver *driver)
{
    uint8_t readData = 0x00;
    uint8_t wrData = reg;
    i2c_transfer transfer;
    
    transfer.device_address = HTS221_DEVICE_ADDRESS;
    transfer.read_data.data_ptr = &readData;
    transfer.read_data.data_size = 1;
    transfer.write_data.data_ptr = &wrData;
    transfer.write_data.data_size = 1;
    i2c_request_transfer(driver,&transfer);

    return readData;
}

static uint8_t HTS221_write_register(uint8_t reg,uint8_t data, i2c_bus_driver *driver)
{
    uint8_t wrData[2];

    wrData[0] = reg;
    wrData[1] = data;

    i2c_transfer transfer;
    
    transfer.device_address = HTS221_DEVICE_ADDRESS;
    transfer.read_data.data_ptr = NULL;
    transfer.read_data.data_size = 0;
    transfer.write_data.data_ptr = wrData;
    transfer.write_data.data_size = 2;
    i2c_request_transfer(driver,&transfer);

    return 0;
}

static inline void HTS221_read_calibration(HTS221_driver_t *drv)
{
    uint8_t data[2];
    i2c_bus_driver* i2c = drv->driver;

    data[0] = HTS221_read_register(REG_T0T1_deg_MSB_x8,i2c);

    drv->T0_deg_C = HTS221_read_register(REG_T0_deg_C_x8,i2c) + ((data[0] & 0x3) << 8);
    drv->T1_deg_C = HTS221_read_register(REG_T1_deg_C_x8,i2c) + ((data[0] & 0xC) << 6);
    
    data[0] = HTS221_read_register(REG_T0_out_LO,i2c);
    data[1] = HTS221_read_register(REG_T0_out_HI,i2c);
    drv->T0_OUT = *((int16_t*)data);

    data[0] = HTS221_read_register(REG_T1_out_LO,i2c);
    data[1] = HTS221_read_register(REG_T1_out_HI,i2c);
    drv->T1_OUT = *((int16_t*)data);

}

void HTS221_run_device(HTS221_driver_t *drv)
{
    // correct it to look nicer
    HTS221_write_register(REG_CTRL_REG1,(1 << 7) | (1 << 2) | 1,drv->driver);
}
