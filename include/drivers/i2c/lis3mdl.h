#ifndef _LIS3MDL_H__
#define _LIS3MDL_H__

#include <stdint.h>
#include "i2c_bus.h"

typedef struct LIS3MDL_driver_data
{
    i2c_bus_driver* driver;
} LIS3MDL_driver_t;

int     LIS3MDL_create_device(i2c_bus_driver *i2c_drv, LIS3MDL_driver_t *drv);
int16_t LIS3MDL_get_magnetic_field_X(LIS3MDL_driver_t *drv);
int16_t LIS3MDL_get_magnetic_field_Y(LIS3MDL_driver_t *drv);
int16_t LIS3MDL_get_magnetic_field_Z(LIS3MDL_driver_t *drv);
void    LIS3MDL_run_device(LIS3MDL_driver_t *drv);

#endif