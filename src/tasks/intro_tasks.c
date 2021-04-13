#include <stdio.h>
#include <intro_tasks.h>
#include <gpio.h>
#include <utils.h>

static void task_producer_run(void* pvParameters);
static void task_consumer_run(void* pvParameters);
static void task_async_flasher_run(void* pvParameters);

static intro_tasks itasks;

/*
    This is stupid not needed comment 
    
    This is producer task which sends notification to 
    consumer task.

    Why do you think this comment is stupid?
*/
static void task_producer_run( void * pvParameters )
{
    UNUSED(pvParameters);
    for( ;; )
    {
        vTaskDelay(PRODUCER_TASK_PERIOD_TICK);
        gpio_toggle(GPIOC,GPIO9);
        xTaskNotifyGive(itasks.consumer_handler);
    }
}

static void task_consumer_run( void * pvParameters )
{
    UNUSED(pvParameters);
    for( ;; )
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        vTaskDelay(CONSUMER_TASK_PERIOD_TICK);
        printf("Hi with delay\n");
    }
}

// MODIFY ME !!
static void task_async_flasher_run(void* pvParameters)
{
    int blinkInteval = 150;
    bool direction = true;
    UNUSED(pvParameters);
    
    for ( ;; ) 
    {
        gpio_toggle(GPIOB,GPIO14);
        vTaskDelay(blinkInteval / portTICK_PERIOD_MS);
        
        if (direction) {
            blinkInteval += 5;
            if (blinkInteval > 250) {
                direction = false;
            }
        } else {
            blinkInteval -= 5;
            if (blinkInteval < 20) {
                direction = true;
            }
        }

    }
}

int create_intro_tasks(intro_tasks** tasks_ptr)
{
    BaseType_t result = pdPASS;
    int ret = -1;
    
    result = xTaskCreate(task_producer_run,"Producer",
                      PRODUCER_TASK_STACK,NULL,
                      tskIDLE_PRIORITY,
                      &itasks.producer_handler);

    result &= xTaskCreate(task_consumer_run,"Consumer",
                       CONSUMER_TASK_STACK,NULL,
                       tskIDLE_PRIORITY,
                       &itasks.consumer_handler);

    result &= xTaskCreate(task_async_flasher_run,"Flasher",
                       CONSUMER_TASK_STACK,NULL,
                       tskIDLE_PRIORITY,
                       &itasks.async_flasher_handler);

    if (pdPASS == result) {
        *tasks_ptr = &itasks;
        itasks.is_initialized = true;
        ret = 0;
    } else {
        itasks.is_initialized = false;
    }

    return ret;    
}