#ifndef _DOSER_TASKS_H__
#define _DOSER_TASKS_H__

#include <FreeRTOS.h>
#include <task.h>
#include <stdint.h>
#include <stdbool.h>

#define DOSER_TASK_STACK         512     // words
#define APPLICATION_CYCLE        200     // milliseconds

typedef struct app_tasks {
    TaskHandle_t producer_handler;
    TaskHandle_t consumer_handler;
    bool is_initialized:1;
} app_main_tasks;

int create_app_tasks(app_main_tasks** tasks_ptr);

#endif