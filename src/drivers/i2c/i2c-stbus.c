#include "drivers/i2c/i2c_stbus.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/i2c.h>
#include <utils.h>

// Contract internal functions
static int i2c_stm32_initialize(void *prv_data);
static int i2c_stm32_set_mode(void* prv_data, i2c_transfer_mode mode);
static int i2c_stm32_transfer(void *prv_data, i2c_transfer *i2c_transfer);
static inline int i2c_stm32_hardware_setup(i2c_speed speed);
static inline int i2c_stm32_initialize_private_data(stm32_i2c_prv *prv);
static inline int i2c_stm32_populate_driver(i2c_bus_driver * driver);

static stm32_i2c_prv privateData;

static i2c_bus_functions stm32_i2c_fun =
{
    .initialize = i2c_stm32_initialize,
    .set_mode = i2c_stm32_set_mode,
    .transfer = i2c_stm32_transfer
};

int create_i2c_stm32_driver(i2c_bus_driver *driver, i2c_speed speed)
{
    int ret = 0;

    ret = i2c_stm32_hardware_setup(speed);
    if (ret < 0)  
        goto ERROR;       // yes we do still see sometimes goto statements ;)

    ret = i2c_stm32_initialize_private_data(&privateData);
    if (ret < 0) 
        goto ERROR;

    ret = i2c_stm32_populate_driver(driver);
    if (ret < 0) 
        goto ERROR;

    ret = i2c_initialize_bus(driver);

ERROR:
    return ret;
}

static int i2c_stm32_initialize(void * prv_data)
{
    UNUSED(prv_data);
    int ret = 0;
    return ret;
}

static int i2c_stm32_set_mode(void* prv_data, i2c_transfer_mode mode)
{
    UNUSED(prv_data);
    UNUSED(mode);

    int ret = 0;
    return ret;
}

static int i2c_stm32_transfer(void *prv_data, i2c_transfer *transfer)
{
    BaseType_t result = pdPASS;
    stm32_i2c_prv* data = (stm32_i2c_prv*)prv_data;
    
    result = xSemaphoreTake(data->semaphore,portMAX_DELAY);
    
    i2c_transfer7(I2C2,
                  transfer->device_address,
                  transfer->write_data.data_ptr,
                  transfer->write_data.data_size,
                  transfer->read_data.data_ptr,
                  transfer->read_data.data_size);

    result = xSemaphoreGive(data->semaphore);

    return (result == pdPASS) ? 0 : -1;
}

static int i2c_stm32_hardware_setup(i2c_speed speed)
{
    int ret = 0;
    UNUSED(speed);
  
    i2c_reset(I2C2);
    rcc_periph_clock_enable(RCC_I2C2);

    i2c_set_7bit_addr_mode(I2C2);
    i2c_set_speed(I2C2,i2c_speed_sm_100k,rcc_apb1_frequency / 1e6);
    i2c_peripheral_enable(I2C2);
    
    return ret;
}

static inline int i2c_stm32_initialize_private_data(stm32_i2c_prv *prv)
{
    int ret = 0;
    prv->speed = I2C_100kHz;
    prv->transfer_mode = I2C_BLOCKING;
    prv->semaphore = xSemaphoreCreateMutex();
    if (prv->semaphore == NULL)
        ret = -1;
    return ret;
}

static inline int i2c_stm32_populate_driver(i2c_bus_driver * driver)
{
    driver->prv_data = &privateData;
    driver->functions = &stm32_i2c_fun;

    return 0;
}