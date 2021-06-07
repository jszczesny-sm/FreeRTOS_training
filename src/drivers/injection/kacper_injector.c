#include <drivers/injection/kacper_injector.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <stdio.h>
#include <string.h>
#include <utils.h>

#define     INJECTOR_TASK_STACK_MEMORY  128
int         INJECTOR_TASK_LOOP_PERIOD = 40;
#define     DEVICE_NAME_LENGTH          16
const char  DEVICE_NAME[] =             "Kacper injector";
#define     TIMER_SETUP_PERIOD          10000
#define     PATTERN_LENGTH              25

int pattern[PATTERN_LENGTH] = {1, 4, 9, 16, 25, 36, 49, 64, 81, 100, 100, 100, 75, 50, 35, 25, 20, 20, 20, 0, 0, 0, 0, 0, 0};

static void initialize_timer(int initial_time);
static int kacper_injector_initialize(void *prvParam);
static int kacper_injector_start(void *prvParam);
static int kacper_injector_stop(void *prvParam);
static int kacper_injector_set_parameter(injector_config *parameter, void*prvParam);
static int kacper_injector_get_parameter(injector_config *parameter, void*prvParam);
void kacper_injector_step(injector_driver *driver_hook);

static injector_drv_functions kacper_functions = {
    .initialize = kacper_injector_initialize,
    .start = kacper_injector_start,
    .stop = kacper_injector_stop,
    .set_parameter = kacper_injector_set_parameter,
    .get_parameter = kacper_injector_get_parameter
};

static inline uint32_t convert_duty_to_output_value(uint8_t value)
{
    return (value * TIMER_SETUP_PERIOD) / 100;
}

void kacper_injector_step(injector_driver *driver_hook)
{
    kacper_injector_priv* private_data = (kacper_injector_priv*) driver_hook->prvData;

    if(driver_hook->is_running)
    {
        if(private_data->loop_counter >= PATTERN_LENGTH) private_data->loop_counter = 0;

        timer_set_oc_value(TIM15, TIM_OC1, convert_duty_to_output_value(pattern[private_data->loop_counter]));
        private_data->loop_counter++;
    }
    else if(!(driver_hook->is_running) && private_data->loop_counter != PATTERN_LENGTH)
    {
        if(private_data->loop_counter >= PATTERN_LENGTH) private_data->loop_counter = 0;

        timer_set_oc_value(TIM15, TIM_OC1, convert_duty_to_output_value(pattern[private_data->loop_counter]));
        private_data->loop_counter++;
    }
}

static void kacper_injector_task(void *parameter)
{
    injector_driver* inj_parameter = (injector_driver*) parameter;

    while(true)
    {
        kacper_injector_step(inj_parameter);
        vTaskDelay(INJECTOR_TASK_LOOP_PERIOD / portTICK_PERIOD_MS);
    }    
}

int kacper_injector_create(injector_driver *driver_hook, int initial_config)
{
    int to_return = 0;
    kacper_injector_priv* private_data = (kacper_injector_priv*) driver_hook->prvData;
    for(int i = 0; i < DEVICE_NAME_LENGTH; i++) driver_hook->device_name[i] = 0x00;
    memcpy(driver_hook->device_name, DEVICE_NAME, sizeof(DEVICE_NAME)/sizeof(char));
    driver_hook->is_initialized = false;
    driver_hook->is_running = false;
    driver_hook->functions = &kacper_functions;
    driver_hook->prvData = pvPortMalloc(sizeof(kacper_injector_priv));

    private_data->initial_open_time = initial_config;
    private_data->loop_counter = 0;

    xTaskCreate(kacper_injector_task, "InjectorDriver",
                INJECTOR_TASK_STACK_MEMORY, (void*)driver_hook,
                tskIDLE_PRIORITY,
                &private_data->injector_task_handle);
                
    return to_return;
}

static int kacper_injector_initialize(void *prvParam)
{
    kacper_injector_priv* private_data = (kacper_injector_priv*) prvParam;

    rcc_periph_clock_enable(RCC_GPIOB);
    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO14);
    gpio_set_af(GPIOB, GPIO_AF14, GPIO14);
    gpio_set_output_options(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_MED, GPIO14);
    initialize_timer(private_data->initial_open_time);

    printf("Injector initialized\n");
    return 0;
}

static int kacper_injector_start(void *prvParam)
{
    UNUSED(prvParam);
    printf("Injector started\n");
    return 0;
}
static int kacper_injector_stop(void *prvParam)
{
    UNUSED(prvParam);
    printf("Injector stopped\n");
    return 0;
}
static int kacper_injector_set_parameter(injector_config *parameter, void* prvParam)
{
    UNUSED(prvParam);
    UNUSED(parameter);
    return 0;
}
static int kacper_injector_get_parameter(injector_config *parameter, void* prvParam)
{
    UNUSED(prvParam);
    UNUSED(parameter);

    return 0;
}

static void initialize_timer(int initial_time)
{
    rcc_periph_clock_enable(RCC_TIM15); //16MHz

    // timer_direction_down(TIM15);
    timer_enable_preload(TIM15);
    
    timer_set_clock_division(TIM15, TIM_CR1_CKD_CK_INT_MUL_2);
    //CKD clock division, clock = 2* ck_int
    timer_set_prescaler(TIM15, 15); //always x-1 

    //Counter max value 65355
    timer_set_counter(TIM15, TIMER_SETUP_PERIOD);
    timer_set_period(TIM15, TIMER_SETUP_PERIOD);

    //Set pwm value to initial_time
    timer_set_oc_value(TIM15, TIM_OC1, initial_time);
    timer_set_oc_polarity_high(TIM15, TIM_OC1);
    timer_set_oc_mode(TIM15,TIM_OC1, TIM_OCM_PWM1);
    timer_enable_break_main_output(TIM15);
    timer_enable_oc_output(TIM15,TIM_OC1);
    timer_enable_counter(TIM15);
}
