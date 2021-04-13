#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <serial.h>
#include <bsp.h>
#include <intro_tasks.h>

static intro_tasks* tasks_holder;

int main(void)
{
    int ret = 0;

    configure_system();
    serial_setup();

    ret = create_intro_tasks(&tasks_holder);

    if (0 > ret) {
        printf("Cannot create requested tasks\n");
    } else {
        vTaskStartScheduler();
    }

    while(1);
     
    return 0;
}