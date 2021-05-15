#include <bsp.h>
#include <stdint.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/gpio.h>

uint32_t SystemCoreClock;

static void configure_flash_memory(void);
static void configure_system_clock(void);
static void configure_led_gpios(void);
static void configure_i2c_gpios(void);

static void configure_flash_memory()
{
   flash_prefetch_enable();
   flash_set_ws(4);
   flash_dcache_enable();
   flash_icache_enable();
}

static void configure_system_clock()
{
   rcc_osc_on(RCC_HSI16);

   rcc_set_main_pll(RCC_PLLCFGR_PLLSRC_HSI16,
                  PLL_DIVISOR,
                  PLL_MULTIPLIER,
                  RCC_PLLCFGR_PLLP_DIV7,
                  RCC_PLLCFGR_PLLQ_DIV4,
                  RCC_PLLCFGR_PLLR_DIV4
                  );
   rcc_osc_on(RCC_PLL);

   rcc_set_sysclk_source(RCC_CFGR_SW_PLL);
   rcc_wait_for_sysclk_status(RCC_PLL);

   rcc_set_clock48_source(RCC_CCIPR_CLK48SEL_PLL);

   rcc_ahb_frequency = AHB_FREQUENCY;
   rcc_apb1_frequency = APB1_FREQUENCY;
   rcc_apb2_frequency = APB2_FREQUENCY;
   SystemCoreClock = SYSTEM_CORE_FREQUENCY;
}

static void configure_led_gpios()
{
   rcc_periph_clock_enable(RCC_GPIOA);
   rcc_periph_clock_enable(RCC_GPIOC);

   gpio_mode_setup(GPIOA,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO5);
   gpio_mode_setup(GPIOC,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO9); 
}

static void configure_i2c_gpios()
{
   // configure I2C2 bus
   rcc_periph_clock_enable(RCC_GPIOB);
   gpio_mode_setup(GPIOB,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO10);
   gpio_mode_setup(GPIOB,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO11);
   gpio_set_af(GPIOB,GPIO_AF4,GPIO10);
   gpio_set_af(GPIOB,GPIO_AF4,GPIO11);
}

void configure_system()
{
   configure_flash_memory();
   configure_system_clock();
   configure_led_gpios();
   configure_i2c_gpios();
}