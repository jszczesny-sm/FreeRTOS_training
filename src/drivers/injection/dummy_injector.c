#include <drivers/injection/dummy_injector.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <stdio.h>
#include <string.h>
#include <utils.h>

static int dummy_injector_initialize(void*);
static int dummy_injector_start(void*);
static int dummy_injector_stop (void*);
static int dummy_injector_set_parameter(injector_config *param,void*);
static int dummy_injector_get_parameter(injector_config *param,void*);
static long dummy_injector_get_total_open_time(void*);
static void initialize_timer(void);

#define INJECTOR_TASK_STACK_MEMORY      128
#define INJECTOR_TASK_LOOP_PERIOD       80

#define HEARTBREAT_PATTERN_LENGTH       16
#define TIMER_SETUP_PERIOD              10000
#define PRESCALER_SETUP(x)              ((x) - 1)

static uint8_t heartbeat[HEARTBREAT_PATTERN_LENGTH] = {0,30,80,50,20,50,80,20,10,0,0,0,0,0,0,0};

static inline uint32_t convert_ducy_to_output_compare(uint8_t value)
{
    return (value * TIMER_SETUP_PERIOD) / 100;
}

static injector_drv_functions dummy_functions = {
    .initialize = dummy_injector_initialize,
    .start = dummy_injector_start,
    .stop = dummy_injector_stop,
    .set_parameter = dummy_injector_set_parameter,
    .get_parameter = dummy_injector_get_parameter
};

static void dummy_injector_task(void *parameter)
{
    injector_driver* inj = (injector_driver*) parameter;

    for(;;) {
        dummy_injector_step(inj);
        vTaskDelay(INJECTOR_TASK_LOOP_PERIOD / portTICK_PERIOD_MS);
    }
}

int dummy_injector_create(injector_driver *drv, int sample_config)
{
    int ret = 0;
    dummy_injector_prv* private_data = (dummy_injector_prv*) drv->prvData;

    memset(drv->device_name,0x00,HEARTBREAT_PATTERN_LENGTH);
    memcpy(drv->device_name,"Dummy injector",15);
    drv->is_initialized = false;
    drv->is_running = false;
    drv->functions = &dummy_functions;
    drv->prvData = pvPortMalloc(sizeof(dummy_injector_prv));

    private_data->initial_open_time = sample_config;
    private_data->loop_counter = 0;
    xTaskCreate(dummy_injector_task,"InjectorDriver",
                INJECTOR_TASK_STACK_MEMORY,(void*)drv,
                tskIDLE_PRIORITY,
                &private_data->injector_task_handle);

    return ret;
}


void dummy_injector_step(injector_driver *drv)
{
    dummy_injector_prv* private_data = (dummy_injector_prv*) drv->prvData;

    if (drv->is_running) {
        // printf("Here I do my steps %ld!\n", 
        //     ((dummy_injector_prv*)drv)->initial_open_time++);
        if (++private_data->loop_counter >= HEARTBREAT_PATTERN_LENGTH) {
            private_data->loop_counter = 0;
        }
        timer_set_oc_value(TIM15, TIM_OC1, 
                           convert_ducy_to_output_compare(heartbeat[private_data->loop_counter]));
    } else {
        printf("Dummy is waiting\n");
    }
}

static void initialize_timer()
{
    rcc_periph_clock_enable(RCC_TIM15);

    timer_direction_down(TIM15);
    timer_enable_preload(TIM15);
    
    timer_set_clock_division(TIM15,TIM_CR1_CKD_CK_INT_MUL_2);
    timer_set_prescaler(TIM15,PRESCALER_SETUP(16));

    timer_set_counter(TIM15,TIMER_SETUP_PERIOD);
    timer_set_period(TIM15,TIMER_SETUP_PERIOD);

    timer_set_oc_value(TIM15,TIM_OC1,(TIMER_SETUP_PERIOD / 10));
    timer_set_oc_polarity_high(TIM15,TIM_OC1);
    timer_set_oc_mode(TIM15,TIM_OC1,TIM_OCM_PWM1);
    timer_enable_break_main_output(TIM15);
    timer_enable_oc_output(TIM15,TIM_OC1);
    timer_enable_counter(TIM15);
}

static int dummy_injector_initialize(void *prvData)
{
    UNUSED(prvData);

    rcc_periph_clock_enable(RCC_GPIOB);
    gpio_mode_setup(GPIOB,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO14);
    gpio_set_af(GPIOB,GPIO_AF14,GPIO14);
    gpio_set_output_options(GPIOB,GPIO_OTYPE_PP,GPIO_OSPEED_MED,GPIO14);
    initialize_timer();

    printf("Dummy initialize\n");
    return 0;
}

static int dummy_injector_start(void *prvData)
{
    UNUSED(prvData);
    return printf("Dummy start\n");
    
    return 0;
}

static int dummy_injector_stop (void *prvData)
{
    UNUSED(prvData);
    return 0;
}

static int dummy_injector_set_parameter(injector_config *param, void *prvData)
{
    UNUSED(param);
    UNUSED(prvData);
    
    return 0;
}

static int dummy_injector_get_parameter(injector_config *param, void *prvData)
{
    UNUSED(param);
    UNUSED(prvData);
    
    return 0;
}

static long dummy_injector_get_total_open_time(void *prvData)
{
    return ((dummy_injector_prv*)prvData)->initial_open_time;
}
