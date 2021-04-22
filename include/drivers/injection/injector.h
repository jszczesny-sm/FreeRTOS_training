#ifndef _INJECTOR_H__
#define _INJECTOR_H__

#include <stdint.h>
#include <stdbool.h>

typedef enum eInjectorParam
{
    DOSING_FREQUENCY,
    PEAK_DURATION,
    HOLD_DURATION,
    HOLD_DUTY
} injector_parameter;

typedef enum eInjectorAction
{
    INJECTOR_START,
    INJECTOR_STOP
} injector_action;

typedef struct config_parameter 
{
    injector_parameter parameter;
    uint16_t value;
} injector_config;

typedef struct injector_drv_fun
{
    int (*initialize)(void* prvData);
    int (*start)(void* prvData);
    int (*stop)(void* prvData);
    int (*set_parameter)(injector_config* param, void* prvData);
    int (*get_parameter)(injector_config* param, void* prvData);
    long (*get_total_open_time)(void* prvData);
} injector_drv_functions;

typedef struct injector_drv
{
    int parameters_supported;
    uint8_t device_name[16];
    bool is_initialized;
    bool is_running;
    injector_drv_functions *functions;
    void *prvData;
} injector_driver;


int injector_init(injector_driver* drv);
int injector_execute(injector_driver* drv, injector_action action);
int injector_set_parameter(injector_driver* drv, injector_config*);
int injector_get_parameter(injector_driver* drv, injector_config*);

#endif
