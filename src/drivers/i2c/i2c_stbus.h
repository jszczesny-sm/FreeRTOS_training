#ifndef _I2C_BUS_H__
#define _I2C_BUS_H__

#include <stdint.h>
#include "i2c_bus.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "stdbool.h"

typedef struct i2c_stm32_private_data {
    xSemaphoreHandle    semaphore;
    i2c_speed           speed;
    i2c_transfer_mode   transfer_mode;
    uint32_t            physical_device;
} stm32_i2c_prv;

int create_i2c_stm32_driver(i2c_bus_driver *driver, i2c_speed speed);

#endif