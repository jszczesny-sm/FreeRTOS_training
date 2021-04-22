#ifndef _DUMMY_INJECTOR_H_
#define _DUMMY_INJECTOR_H_

#include <drivers/injection/injector.h>
#include <FreeRTOS.h>
#include <task.h>

typedef struct dummy_injector_private {
    long initial_open_time;
    TaskHandle_t injector_task_handle;
    int          loop_counter;
} dummy_injector_prv;

int  dummy_injector_create(injector_driver* drv, int sample_config);
void dummy_injector_step(injector_driver* drv);

#endif