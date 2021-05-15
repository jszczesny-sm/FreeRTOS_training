#include <drivers/i2c/i2c_bus.h>
#include <utils.h>

int i2c_initialize_bus(i2c_bus_driver* driver)
{
    return driver->functions->initialize(driver->prv_data);
}

int i2c_set_transfer_mode(i2c_bus_driver* driver,i2c_transfer_mode mode)
{
    UNUSED(driver);
    UNUSED(mode);
    return 0;
}

int i2c_request_transfer(i2c_bus_driver* driver, i2c_transfer* transfer)
{
    return driver->functions->transfer(driver->prv_data,transfer);
}
