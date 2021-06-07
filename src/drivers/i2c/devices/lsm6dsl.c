#include <FreeRTOS.h>
#include <drivers/i2c/lsm6dsl.h>
#include <stdint.h>
#include <utils.h>
#include <stdio.h>

static inline uint8_t LSM6DSL_get_device_id(i2c_bus_driver *driver);
static uint8_t LSM6DSL_read_register(uint8_t reg, i2c_bus_driver *driver);
static uint16_t LSM6DSL_read_register_16(uint16_t reg, i2c_bus_driver *driver);
static uint8_t LSM6DSL_write_register(uint8_t reg,uint8_t data, i2c_bus_driver *driver);
static inline void LSM6DSL_default_settings(i2c_bus_driver *i2c_drv, LSM6DSL_driver_t *drv);
static int16_t LSM6DSL_read_raw_temperature(LSM6DSL_driver_t *drv);

static inline  uint8_t LSM6DSL_get_device_id(i2c_bus_driver *driver)
{
    return LSM6DSL_read_register(REG_WHO_AM_I,driver);
}

static uint8_t LSM6DSL_read_register(uint8_t reg, i2c_bus_driver *driver)
{
    uint8_t readData = 0x00;
    uint8_t wrData = reg;
    i2c_transfer transfer;
    
    transfer.device_address = LSM6DSL_DEVICE_ADDRESS;
    transfer.read_data.data_ptr = &readData;
    transfer.read_data.data_size = 1;
    transfer.write_data.data_ptr = &wrData;
    transfer.write_data.data_size = 1;
    i2c_request_transfer(driver,&transfer);

    return readData;
}

static uint16_t LSM6DSL_read_register_16(uint16_t reg, i2c_bus_driver *driver)
{
    uint16_t readData = 0x00;
    uint16_t wrData = reg;
    i2c_transfer transfer;
    
    transfer.device_address = LSM6DSL_DEVICE_ADDRESS;
    transfer.read_data.data_ptr = &readData;
    transfer.read_data.data_size = 2;
    transfer.write_data.data_ptr = &wrData;
    transfer.write_data.data_size = 1;
    i2c_request_transfer(driver,&transfer);

    return readData;
}

static uint8_t LSM6DSL_write_register(uint8_t reg,uint8_t data, i2c_bus_driver *driver)
{
    uint8_t wrData[2] = {0x00};

    wrData[0] = reg;
    wrData[1] = data;

    i2c_transfer transfer;
    
    transfer.device_address = LSM6DSL_DEVICE_ADDRESS;
    transfer.read_data.data_ptr = NULL;
    transfer.read_data.data_size = 0;
    transfer.write_data.data_ptr = wrData;
    transfer.write_data.data_size = 2;
    i2c_request_transfer(driver,&transfer);

    return 0;
}

static inline void LSM6DSL_default_settings(i2c_bus_driver *i2c_drv, LSM6DSL_driver_t *drv)
{
    uint8_t data_to_write = 0;
    uint8_t data_to_read = 0;

    //Setup the accelerometer******************************

    data_to_write = 0;

    //Accel range = 2g
    data_to_write |= LSM6DS3_ACC_GYRO_FS_XL_2g;
    drv->accel_range = 2;

    //Accel sample rate = 104 Hz
    data_to_write |= LSM6DS3_ACC_GYRO_ODR_XL_104Hz;
    drv->accel_sample_rate = 104;

    //Accel anti-aliasing filter bandwidth = 400 Hz
    data_to_write |= LSM6DS3_ACC_GYRO_BW_XL_400Hz;
    drv->accel_band_width = 400;

    //Write the patched together data
	LSM6DSL_write_register(LSM6DS3_ACC_GYRO_CTRL1_XL, data_to_write, i2c_drv);

    
    // //Setup the gyroscope******************************

    data_to_write = 0;

    //Gyro enabled = 1
    data_to_write |= LSM6DS3_ACC_GYRO_FS_125_ENABLED;
    drv->gyro_enabled = 1;
    
    //Gyro range = 250 dps
    data_to_write |= LSM6DS3_ACC_GYRO_FS_G_250dps;
    drv->gyro_range = 250;
    
    //Gyro sample rate = 416 Hz
    data_to_write |= LSM6DS3_ACC_GYRO_ODR_G_416Hz;
    drv->gyro_sample_rate = 416;
    
    //Gyro anti-aliasing filter bandwidth by default is 400 Hz
    drv->gyro_band_width = 400;

    //Write the patched together data
    LSM6DSL_write_register(LSM6DS3_ACC_GYRO_CTRL2_G, data_to_write, i2c_drv);

}

int LSM6DSL_create_device(i2c_bus_driver *i2c_drv, LSM6DSL_driver_t *drv)
{
    int ret = 0;
    int device_id = 0;

    device_id = LSM6DSL_get_device_id(i2c_drv);
    printf("device id: %x\n", device_id);
    if (device_id == LSM6DSL_DEVICE_ADDRESS) {
        drv->driver = i2c_drv;
        printf("LSM6DSL alive\n");
        LSM6DSL_default_settings(i2c_drv, drv);
    } else {
        ret = -1;
    }

    return ret;
}

int16_t LSM6DSL_read_raw_gyro_x(LSM6DSL_driver_t *drv) 
{
    int16_t raw_gyro_x = 0;
    raw_gyro_x = (int16_t)LSM6DSL_read_register_16(LSM6DS3_ACC_GYRO_OUTX_L_G, drv->driver);
    return raw_gyro_x;
}

int16_t LSM6DSL_read_raw_accel_x(LSM6DSL_driver_t *drv) 
{
    int16_t raw_accel_x = 0;
    raw_accel_x = (int16_t)LSM6DSL_read_register_16(LSM6DS3_ACC_GYRO_OUTX_L_XL, drv->driver);
    return raw_accel_x;
}

int16_t LSM6DSL_read_raw_accel_y(LSM6DSL_driver_t *drv) 
{
    int16_t raw_accel_y = 0;
    raw_accel_y = (int16_t)LSM6DSL_read_register_16(LSM6DS3_ACC_GYRO_OUTY_L_XL, drv->driver);
    return raw_accel_y;
}

int16_t LSM6DSL_read_raw_accel_z(LSM6DSL_driver_t *drv) 
{
    int16_t raw_accel_z = 0;
    raw_accel_z = (int16_t)LSM6DSL_read_register_16(LSM6DS3_ACC_GYRO_OUTZ_L_XL, drv->driver);
    return raw_accel_z;
}

int16_t LSM6DSL_read_raw_gyro_y(LSM6DSL_driver_t *drv) 
{
    int16_t raw_gyro_y = 0;
    raw_gyro_y = (int16_t)LSM6DSL_read_register_16(LSM6DS3_ACC_GYRO_OUTY_L_G, drv->driver);
    return raw_gyro_y;
}

int16_t LSM6DSL_read_raw_gyro_z(LSM6DSL_driver_t *drv) 
{
    int16_t raw_gyro_z = 0;
    raw_gyro_z = (int16_t)LSM6DSL_read_register_16(LSM6DS3_ACC_GYRO_OUTZ_L_G, drv->driver);
    return raw_gyro_z;
}

int16_t LSM6DSL_read_raw_temperature(LSM6DSL_driver_t *drv) 
{
    int16_t temperature = 0;
    temperature = (int16_t)LSM6DSL_read_register_16(LSM6DS3_ACC_GYRO_OUT_TEMP_L, drv->driver);
    return temperature;
}

float LSM6DSL_calc_accel(int16_t input, LSM6DSL_driver_t *drv)
{
    float accel_in_g = 0;
    accel_in_g = (float)input * 0.000061;   // linear equation based on the table on p. 25
    if (accel_in_g > drv->accel_range) {
        accel_in_g = drv->accel_range;
    }
    if (accel_in_g < (-1)*(drv->accel_range)) {
            accel_in_g = (-1)*(drv->accel_range);
    }
    return accel_in_g;
}

float LSM6DSL_calc_gyro(int16_t input, LSM6DSL_driver_t *drv)
{
    float gyro_in_dps = 0;
    gyro_in_dps = (float)input * 0.00875;   // linear equation based on the table on p. 25
    if (gyro_in_dps > drv->gyro_range) {
        gyro_in_dps = drv->gyro_range;
    }
    if (gyro_in_dps < (-1)*(drv->gyro_range)) {
            gyro_in_dps = (-1)*(drv->gyro_range);
    }
    return gyro_in_dps;
}

float LSM6DSL_read_temp_C(LSM6DSL_driver_t *drv) 
{
    float output = 0;
	output = (float)LSM6DSL_read_raw_temperature(drv);
    output = 0.0039 * output + 25;          // linear equation based on the table on p. 95
	return output;                          // in aplication note
}
