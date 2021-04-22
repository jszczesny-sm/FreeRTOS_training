#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <serial.h>
#include <bsp.h>
#include <intro_tasks.h>
#include <injection/dummy_injector.h>

static intro_tasks* tasks_holder;
static injector_driver my_first_injector;

int main(void)
{
    int ret = 0;

    configure_system();
    serial_setup();

    ret = create_intro_tasks(&tasks_holder);
    ret |= dummy_injector_create(&my_first_injector,123);
    
    injector_init(&my_first_injector);
    injector_execute(&my_first_injector,INJECTOR_START);

    if (0 > ret) {
        printf("Cannot create requested tasks\n");
    } else {
        vTaskStartScheduler();
    }

    while(1);
     
    return 0;
}