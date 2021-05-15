#ifndef _HTS221_H__
#define _HTS221_H__

#include <stdint.h>
#include "i2c_bus.h"

typedef struct HTS221_driver_data
{
    uint16_t T0_deg_C;
    uint16_t T1_deg_C;
    int16_t T0_OUT;
    int16_t T1_OUT;
    
    i2c_bus_driver* driver;
} HTS221_driver_t;

int     HTS221_create_device(i2c_bus_driver *i2c_drv, HTS221_driver_t *drv);
int16_t HTS221_get_temperature(HTS221_driver_t *drv);
void    HTS221_run_device(HTS221_driver_t *drv);

#endif