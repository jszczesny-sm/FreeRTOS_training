#ifndef __KACPER_INJECTOR_H
#define __KACPER_INJECTOR_H

#include <drivers/injection/injector.h>
#include <FreeRTOS.h>
#include <task.h>

typedef struct kacper_injector_private
{
    int initial_open_time;
    int loop_counter;
    TaskHandle_t injector_task_handle;
}kacper_injector_priv;

int kacper_injector_create(injector_driver *driver_hook, int initial_config);

#endif