#include <drivers/injection/konrad_injector.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <stdio.h>
#include <string.h>
#include <utils.h>

static int konrad_injector_initialize(void*);
static int konrad_injector_start(void*);
static int konrad_injector_stop (void*);
static int konrad_injector_set_parameter(injector_config *param,void*);
static int konrad_injector_get_parameter(injector_config *param,void*);
static void initialize_timer(void);

#define INJECTOR_TASK_STACK_MEMORY      128
#define INJECTOR_TASK_LOOP_PERIOD       120

#define DEVICE_NAME_LENGTH              17

#define PEAK_AND_HOLD_PATTERN_LENGTH    16
#define TIMER_SETUP_PERIOD              10000
#define PRESCALER_SETUP(x)              ((x) - 1)

static uint8_t peak_and_hold[PEAK_AND_HOLD_PATTERN_LENGTH] = {0,80,80,50,30,10,10,10,10,10,10,0,0,0,0,0};
static injector_config my_config;

static inline uint32_t convert_ducy_to_output_compare(uint8_t value)
{
    return (value * TIMER_SETUP_PERIOD) / 100;
}

static injector_drv_functions konrad_functions = {
    .initialize = konrad_injector_initialize,
    .start = konrad_injector_start,
    .stop = konrad_injector_stop,
    .set_parameter = konrad_injector_set_parameter,
    .get_parameter = konrad_injector_get_parameter
};

static void konrad_injector_task(void *parameter)
{
    injector_driver* inj = (injector_driver*) parameter;

    for(;;) {
        konrad_injector_step(inj);
        vTaskDelay(INJECTOR_TASK_LOOP_PERIOD / portTICK_PERIOD_MS);
    }
}

int konrad_injector_create(injector_driver *drv, int in_open_time, char en_code[])
{
    int ret = 0;
    konrad_injector_prv* private_data = NULL;

    memset(drv->device_name,0x00,DEVICE_NAME_LENGTH);
    memcpy(drv->device_name,"Konrad injector",16);
    drv->is_initialized = false;
    drv->is_running = false;
    drv->functions = &konrad_functions;
    drv->prvData = pvPortMalloc(sizeof(konrad_injector_prv));
    private_data = (konrad_injector_prv*) drv->prvData;

    if (private_data != NULL) {
        private_data->state = IDLE;
        private_data->work_time_s = 0.0;
        private_data->initial_open_time = in_open_time;
        private_data->loop_counter = 0;
        memset(private_data->engin_code,0x00,SIZE_OF_ENGINE_CODE);
        memcpy(private_data->engin_code,en_code,strlen(en_code));
        xTaskCreate(konrad_injector_task,"KonradInjectorDriver",
                    INJECTOR_TASK_STACK_MEMORY,(void*)drv,
                    tskIDLE_PRIORITY,
                    &private_data->konrad_injector_task_handle);

    } else {
        ret = -1;
    }

    return ret;

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

void konrad_injector_step(injector_driver *drv)
{
    konrad_injector_prv* private_data = (konrad_injector_prv*) drv->prvData;
    static bool flag;
    static float total_work_time_s = 0;

    if (drv->is_running) {
        flag = 0;
        // printf("Here I do my steps %ld!\n", 
        //     ((konrad_injector_prv*)drv)->initial_open_time++);
        if (++private_data->loop_counter >= PEAK_AND_HOLD_PATTERN_LENGTH) {
            private_data->loop_counter = 0;
        }
        timer_set_oc_value(TIM15, TIM_OC1, 
                           convert_ducy_to_output_compare(peak_and_hold[private_data->loop_counter]));
        total_work_time_s += ((float)INJECTOR_TASK_LOOP_PERIOD)/1000; 
        private_data->work_time_s = total_work_time_s;

    } else {
        if(flag == 0) {
            flag = 1;
            printf("Konrad_injector is waiting...\r\n");
            printf("Total work time: %.2f s\n", private_data->work_time_s);
        }
    }
}

static int konrad_injector_initialize(void *prvData)
{
    UNUSED(prvData);

    rcc_periph_clock_enable(RCC_GPIOB);
    gpio_mode_setup(GPIOB,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO14);
    gpio_set_af(GPIOB,GPIO_AF14,GPIO14);
    gpio_set_output_options(GPIOB,GPIO_OTYPE_PP,GPIO_OSPEED_MED,GPIO14);
    initialize_timer();

    printf("\nKonrad injector initialize\n");
    return 0;
}

static int konrad_injector_start(void *prvData)
{
    UNUSED(prvData);
    printf("\nKonrad injector start\n");    
    return 0;
}

static int konrad_injector_stop (void *prvData)
{
    UNUSED(prvData);
    printf("Konrad injector stop\n");
    return 0;
}

static int konrad_injector_set_parameter(injector_config *param, void *prvData)
{
    param->value = prvData;
    UNUSED(param);
    UNUSED(prvData);
    
    return 0;
}

static int konrad_injector_get_parameter(injector_config *param, void *prvData)
{
    UNUSED(param);
    UNUSED(prvData);
    
    return 0;
}
