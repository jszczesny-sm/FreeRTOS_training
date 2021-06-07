#include <drivers/injection/korneliusz_injector.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <stdio.h>
#include <string.h>
#include <utils.h>

static int korneliusz_injector_initialize(void*);
static int korneliusz_injector_start(void*);
static int korneliusz_injector_stop (void*);
static int korneliusz_injector_set_parameter(injector_config *param,void*);
static int korneliusz_injector_get_parameter(injector_config *param,void*);
static long korneliusz_injector_get_total_open_time(void*);
static void initialize_timer(void);

#define INJECTOR_TASK_STACK_MEMORY      128
#define INJECTOR_TASK_LOOP_PERIOD       100
#define INJECTOR_FLOW_INTENSITY_MODES   5
#define CHANGE_MODE_AFTER_LOOPS         3
#define FLOW_PATTERN_LENGTH             30
#define TIMER_SETUP_PERIOD              10000
#define PRESCALER_SETUP(x)              ((x) - 1)

static uint8_t flow[INJECTOR_FLOW_INTENSITY_MODES][FLOW_PATTERN_LENGTH] = {{0,0,0,60,45,30,20,10,10,10,10,10,10,10,10,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                                                           {0,0,0,70,55,45,35,25,25,25,25,25,25,25,25,25,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                                                           {0,0,0,80,60,50,45,40,40,40,40,40,40,40,40,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                                                           {0,0,0,90,75,65,55,50,50,50,50,50,50,50,50,50,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                                                           {0,0,0,95,80,70,60,55,55,55,55,55,55,55,55,55,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};

static inline uint32_t convert_ducy_to_output_compare(uint8_t value)
{
    return (value * TIMER_SETUP_PERIOD) / 100;
}

static injector_drv_functions korneliusz_functions = {
    .initialize = korneliusz_injector_initialize,
    .start = korneliusz_injector_start,
    .stop = korneliusz_injector_stop,
    .set_parameter = korneliusz_injector_set_parameter,
    .get_parameter = korneliusz_injector_get_parameter
};

static void korneliusz_injector_task(void *parameter)
{
    injector_driver* inj = (injector_driver*) parameter;

    for(;;) {
        korneliusz_injector_step(inj);
        vTaskDelay(INJECTOR_TASK_LOOP_PERIOD / portTICK_PERIOD_MS);
    }
}

int korneliusz_injector_create(injector_driver *drv, int initial_flow_mode)
{
    int ret = 0;
    korneliusz_injector_prv* private_data = NULL;

    memset(drv->device_name,0x00,FLOW_PATTERN_LENGTH);
    memcpy(drv->device_name,"Korneliusz's injector",22);
    drv->is_initialized = false;
    drv->is_running = false;
    drv->functions = &korneliusz_functions;
    drv->prvData = pvPortMalloc(sizeof(korneliusz_injector_prv));
    private_data = (korneliusz_injector_prv*) drv->prvData;

    if (NULL != private_data) {
        private_data->flow_mode = initial_flow_mode;
        private_data->loop_counter = 0;
        private_data->full_loop_counter = 0;
        private_data->total_flow = 0;
        xTaskCreate(korneliusz_injector_task, "InjectorDriver",
                    INJECTOR_TASK_STACK_MEMORY, (void*)drv,
                    tskIDLE_PRIORITY,
                    &private_data->injector_task_handle);
    } else {
        ret = -1;
    }
    return ret;
}


void korneliusz_injector_step(injector_driver *drv)
{
    korneliusz_injector_prv* private_data = (korneliusz_injector_prv*) drv->prvData;

    if (drv->is_running) {
        if (++private_data->loop_counter >= FLOW_PATTERN_LENGTH) {
            printf("Total flow units in this cycle: %ld\n", korneliusz_injector_get_total_open_time(private_data));
            private_data->loop_counter = 0;
            private_data->total_flow = 0;
            if (++private_data->full_loop_counter >= CHANGE_MODE_AFTER_LOOPS){
                private_data->full_loop_counter = 0;
                if (++private_data->flow_mode > HIGH_FLOW) private_data->flow_mode = LOW_FLOW;
            }
        }
        timer_set_oc_value(TIM15, TIM_OC1, 
                           convert_ducy_to_output_compare(flow[private_data->loop_counter][private_data->flow_mode]));
        private_data->total_flow += (flow[private_data->flow_mode][private_data->loop_counter] * INJECTOR_TASK_LOOP_PERIOD) / 100;
    } else {
        printf("Korneliusz's injector is waiting\n");
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

static int korneliusz_injector_initialize(void *prvData)
{
    UNUSED(prvData);

    rcc_periph_clock_enable(RCC_GPIOB);
    gpio_mode_setup(GPIOB,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO14);
    gpio_set_af(GPIOB,GPIO_AF14,GPIO14);
    gpio_set_output_options(GPIOB,GPIO_OTYPE_PP,GPIO_OSPEED_MED,GPIO14);
    initialize_timer();

    printf("Korneliusz's injector initialize\n");
    return 0;
}

static int korneliusz_injector_start(void *prvData)
{
    UNUSED(prvData);
    return printf("Korneliusz's injector start\n");
    
    return 0;
}

static int korneliusz_injector_stop (void *prvData)
{
    UNUSED(prvData);
    return 0;
}

static int korneliusz_injector_set_parameter(injector_config *param, void *prvData)
{
    UNUSED(param);
    UNUSED(prvData);
    ((korneliusz_injector_prv*)prvData)->flow_mode = param->value;

    return 0;
}

static int korneliusz_injector_get_parameter(injector_config *param, void *prvData)
{
    UNUSED(param);
    UNUSED(prvData);
    
    return 0;
}

static long korneliusz_injector_get_total_open_time(void *prvData)
{
    return ((korneliusz_injector_prv*)prvData)->total_flow;
}
