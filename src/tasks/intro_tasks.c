#include <stdio.h>
#include <intro_tasks.h>
#include <gpio.h>
#include <utils.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/l4/nvic.h>

static void task_producer_run(void* pvParameters);
static void task_consumer_run(void* pvParameters);
static void task_async_flasher_run(void* pvParameters);

static intro_tasks itasks;

static volatile int counter;

void exti15_10_isr()
{
    BaseType_t higherTaskWoken;
    exti_reset_request(EXTI13);
    if (NULL != itasks.producer_handler) {
        xTaskNotifyFromISR(itasks.producer_handler,0,eIncrement,&higherTaskWoken);
    }
}

static void task_producer_run( void * pvParameters )
{
    UNUSED(pvParameters);
    uint32_t notifications = 0;


    rcc_periph_clock_enable(RCC_GPIOC);
    rcc_periph_clock_enable(RCC_SYSCFG);

    gpio_mode_setup(GPIOC,GPIO_MODE_INPUT,GPIO_PUPD_NONE,GPIO13);
    exti_select_source(EXTI13,GPIOC);
    exti_set_trigger(EXTI13,EXTI_TRIGGER_FALLING);
    exti_enable_request(EXTI13);

    nvic_set_priority(NVIC_EXTI15_10_IRQ,128);
    nvic_enable_irq(NVIC_EXTI15_10_IRQ);

    for( ;; )
    {
        xTaskNotifyWait(pdFALSE,0,&notifications,portMAX_DELAY);
        printf("interrupts %ld\n",notifications);
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
        printf("I am alive\n");
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
        gpio_toggle(GPIOA,GPIO5);
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