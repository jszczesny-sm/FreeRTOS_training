#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <serial.h>
#include <bsp.h>
#include <intro_tasks.h>
#include <doser_application.h>
#include <injection/dummy_injector.h>
#include <drivers/i2c/i2c_stbus.h>
#include <drivers/i2c/hts221.h>

// TASKS
static intro_tasks *tasks_holder;
static app_main_tasks *app_holder;
// DRIVERS
static injector_driver my_first_injector;
static i2c_bus_driver  i2c_internal_driver;

HTS221_driver_t temp_sensor;

static int create_drivers(void);

int main(void)
{
    int ret = 0;

    configure_system();
    serial_setup();
    create_drivers();

    ret = create_intro_tasks(&tasks_holder);
    if (ret < 0)    
        goto ERR;

    ret = create_app_tasks(&app_holder);
    if (ret < 0)
        goto ERR;
    
    vTaskStartScheduler();

ERR:
    printf("Initialization error....\n");
    while(1);
     
    return 0;
}

static int create_drivers(void)
{
    int ret = 0;
    ret |= dummy_injector_create(&my_first_injector,123);
    ret |= create_i2c_stm32_driver(&i2c_internal_driver,I2C_100kHz);
    ret |= HTS221_create_device(&i2c_internal_driver,&temp_sensor);
    injector_init(&my_first_injector);
    injector_execute(&my_first_injector,INJECTOR_START);
    
    return ret;
}