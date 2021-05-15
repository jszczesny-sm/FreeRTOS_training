#include <stdio.h>
#include <gpio.h>
#include <utils.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/l4/nvic.h>

#include <doser_application.h>
#include <drivers/i2c/hts221.h>

static void task_application_run(void *pvParameters);
static void application_process_events(void);
static void application_step(void);

static app_main_tasks itasks;

extern HTS221_driver_t temp_sensor;

static void task_application_run(void *pvParameters)
{
    UNUSED(pvParameters);
    TickType_t xLastWakeTime;
    const TickType_t xPeriod = APPLICATION_CYCLE;
    
    xLastWakeTime = xTaskGetTickCount();
    
    for( ;; )
    {
        vTaskDelayUntil(&xLastWakeTime,xPeriod / portTICK_PERIOD_MS);
        application_process_events();
        application_step();
    }
}

static void application_process_events()
{
    // process all events from other tasks here
}

static void application_step()
{
    printf(".");
    // run main application here ;)
}

int create_app_tasks(app_main_tasks **tasks_ptr)
{
    BaseType_t result = pdPASS;
    int ret = -1;
    
    result = xTaskCreate(task_application_run,"App",
                      DOSER_TASK_STACK,NULL,
                      tskIDLE_PRIORITY,
                      &itasks.producer_handler);

    if (pdPASS == result) {
        *tasks_ptr = &itasks;
        itasks.is_initialized = true;
        ret = 0;
    } else {
        itasks.is_initialized = false;
    }

    return ret;    
}