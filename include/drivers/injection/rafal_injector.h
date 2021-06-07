#ifndef _rafal_INJECTOR_H_
#define _rafal_INJECTOR_H_

#include <drivers/injection/injector.h>
#include <FreeRTOS.h>
#include <task.h>

typedef struct rafal_injector_private {
    long initial_open_time;
    TaskHandle_t injector_task_handle;
    int          loop_counter;
} rafal_injector_prv;

int  rafal_injector_create(injector_driver *drv, int sample_config);
void rafal_injector_step(injector_driver *drv);

#endif