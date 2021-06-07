#include <drivers/injection/jakub_injector.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <stdio.h>
#include <string.h>
#include <utils.h>

static int jakub_injector_initialize(void*);
static int jakub_injector_start(void*);
static int jakub_injector_stop (void*);
static int jakub_injector_set_parameter(injector_config *param,void*);
static int jakub_injector_get_parameter(injector_config *param,void*);
static long jakub_injector_get_total_open_time(void*);
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

static injector_drv_functions jakub_functions = {
    .initialize = jakub_injector_initialize,
    .start = jakub_injector_start,
    .stop = jakub_injector_stop,
    .set_parameter = jakub_injector_set_parameter,
    .get_parameter = jakub_injector_get_parameter
};

static void jakub_injector_task(void *parameter)
{
    injector_driver* inj = (injector_driver*) parameter;

    for(;;) {
        jakub_injector_step(inj);
        vTaskDelay(INJECTOR_TASK_LOOP_PERIOD / portTICK_PERIOD_MS);
    }
}

int jakub_injector_create(injector_driver *drv, int sample_config)
{
    int ret = 0;
    jakub_injector_prv* private_data = NULL;

    memset(drv->device_name,0x00,HEARTBREAT_PATTERN_LENGTH);
    memcpy(drv->device_name,"jakub_injector",15);
    drv->is_initialized = false;
    drv->is_running = false;
    drv->functions = &jakub_functions;
    drv->prvData = pvPortMalloc(sizeof(jakub_injector_prv));
    private_data = (jakub_injector_prv*) drv->prvData;
    
    if (NULL != private_data) {
        private_data->initial_open_time = sample_config;
        private_data->loop_counter = 0;
        private_data->steps_counter = 0;
        private_data->timer2_state = true;
        xTaskCreate(jakub_injector_task,"InjectorDriver",
                    INJECTOR_TASK_STACK_MEMORY,(void*)drv,
                    tskIDLE_PRIORITY,
                    &private_data->injector_task_handle);
    } else {
        ret = -1;
    }
    
    return ret;
}


void jakub_injector_step(injector_driver *drv)
{
    jakub_injector_prv* private_data = (jakub_injector_prv*) drv->prvData;

    if (drv->is_running) {
        if(private_data->initial_open_time >= (100 * private_data->steps_counter)){
            printf("There is above %ld steps\n", 100 * private_data->steps_counter++);
            if (private_data->timer2_state){
                timer_disable_counter(TIM2);
                private_data->timer2_state = false;
            }
            else{ 
                timer_enable_counter(TIM2);
                private_data->timer2_state = true;
            }
        }
        private_data->initial_open_time++;
        if (++private_data->loop_counter >= HEARTBREAT_PATTERN_LENGTH) {
            private_data->loop_counter = 0;
        }
        timer_set_oc_value(TIM15, TIM_OC1, 
                           convert_ducy_to_output_compare(heartbeat[private_data->loop_counter]));

        if (private_data->timer2_state)                  
            timer_set_oc_value(TIM2, TIM_OC1, 
                           convert_ducy_to_output_compare(heartbeat[private_data->loop_counter]));
    } else {
        printf("Injector is waiting\n");
    }
}

static void initialize_timer()
{
    // Timer15 for LED2 (PB14)
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

    // Timer2 for LED1 (PA5)
    rcc_periph_clock_enable(RCC_TIM2);

    timer_direction_down(TIM2);
    timer_enable_preload(TIM2);
    
    timer_set_clock_division(TIM2,TIM_CR1_CKD_CK_INT_MUL_2);
    timer_set_prescaler(TIM2,PRESCALER_SETUP(16));

    timer_set_counter(TIM2,TIMER_SETUP_PERIOD);
    timer_set_period(TIM2,TIMER_SETUP_PERIOD);

    timer_set_oc_value(TIM15,TIM_OC1,(TIMER_SETUP_PERIOD / 10));
    timer_set_oc_polarity_high(TIM2,TIM_OC1);
    timer_set_oc_mode(TIM2,TIM_OC1,TIM_OCM_PWM1);
    timer_enable_break_main_output(TIM2);
    timer_enable_oc_output(TIM2,TIM_OC1);
    timer_enable_counter(TIM2);
}

static int jakub_injector_initialize(void *prvData)
{
    UNUSED(prvData);

    rcc_periph_clock_enable(RCC_GPIOB);
    gpio_mode_setup(GPIOB,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO14);
    gpio_set_af(GPIOB,GPIO_AF14,GPIO14);
    gpio_set_output_options(GPIOB,GPIO_OTYPE_PP,GPIO_OSPEED_MED,GPIO14);

    rcc_periph_clock_enable(RCC_GPIOA);
    gpio_mode_setup(GPIOA,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO5);
    gpio_set_af(GPIOA,GPIO_AF1,GPIO5);
    gpio_set_output_options(GPIOA,GPIO_OTYPE_PP,GPIO_OSPEED_MED,GPIO5);
    
    initialize_timer();

    printf("Injector initialize\n");
    return 0;
}

static int jakub_injector_start(void *prvData)
{
    UNUSED(prvData);
    return printf("Injector start\n");
    
    return 0;
}

static int jakub_injector_stop (void *prvData)
{
    UNUSED(prvData);
    return 0;
}

static int jakub_injector_set_parameter(injector_config *param, void *prvData)
{
    UNUSED(param);
    UNUSED(prvData);
    
    return 0;
}

static int jakub_injector_get_parameter(injector_config *param, void *prvData)
{
    UNUSED(param);
    UNUSED(prvData);
    
    return 0;
}

static long jakub_injector_get_total_open_time(void *prvData)
{
    return ((jakub_injector_prv*)prvData)->initial_open_time;
}
