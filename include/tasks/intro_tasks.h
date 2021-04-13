#ifndef _INTRO_TASKS_H__
#define _INTRO_TASKS_H__

#include <FreeRTOS.h>
#include <task.h>
#include <stdint.h>
#include <stdbool.h>

#define PRODUCER_TASK_PERIOD         500    // milliseconds
#define PRODUCER_TASK_PERIOD_TICK   (PRODUCER_TASK_PERIOD / portTICK_PERIOD_MS)
#define CONSUMER_TASK_PERIOD        140    // milliseconds
#define CONSUMER_TASK_PERIOD_TICK   (PRODUCER_TASK_PERIOD / portTICK_PERIOD_MS)


#define PRODUCER_TASK_STACK         128     // words
#define CONSUMER_TASK_STACK         128     
#define BLINKER_TASK_STACK          128     

typedef struct intro_tasks_t {
    TaskHandle_t producer_handler;
    TaskHandle_t consumer_handler;
    TaskHandle_t async_flasher_handler;
    bool is_initialized:1;
} intro_tasks;

int create_intro_tasks(intro_tasks** tasks_ptr);

#endif