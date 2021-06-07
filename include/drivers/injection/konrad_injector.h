#ifndef _KONRAD_INJECTOR_H_
#define _KONRAD_INJECTOR_H_

#include <drivers/injection/injector.h>
#include <FreeRTOS.h>
#include <task.h>

#define SIZE_OF_ENGINE_CODE 30

typedef enum
{
    IDLE =      0,
    INIT =      1,
    DOSING =    2,
    STOP =      3
} konrad_inj_state;

typedef struct konrad_injector_private {
    konrad_inj_state state;
    long initial_open_time;
    char engin_code[SIZE_OF_ENGINE_CODE];
    int          loop_counter;
    float work_time_s;

    TaskHandle_t konrad_injector_task_handle;
} konrad_injector_prv;

int konrad_injector_create(injector_driver *drv, int in_open_time, char en_code[]);
void konrad_injector_step(injector_driver *drv);

#endif