#ifndef KORNELIUSZ_INJECTOR_H
#define KORNELIUSZ_INJECTOR_H

#include <drivers/injection/injector.h>
#include <FreeRTOS.h>
#include <task.h>

enum flow_mode_e { LOW_FLOW, LOW_MED_FLOW, MED_FLOW, MED_HIGH_FLOW, HIGH_FLOW };

typedef struct korneliusz_injector_private {
    long            total_flow;
    int             flow_mode;
    TaskHandle_t    injector_task_handle;
    int             loop_counter;
    int             full_loop_counter;
} korneliusz_injector_prv;

int  korneliusz_injector_create(injector_driver *drv, int sample_config);
void korneliusz_injector_step(injector_driver *drv);

#endif
