
#include <spi/spi.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>


/* Driver informations */ 

#define ISM_RESET_PIN       GPIO8
#define ISM_RESET_PORT      GPIOE

#define ISM_READY_PIN       GPIO1
#define ISM_READY_PORR      GPIOE

#define ISM_BOOT0_PIN       GPIO12
#define ISM_BOOT0_PORT      GPIOB

#define ISM_BOOT0_WAKEUP    GPIO13
#define ISM_BOOT0_PORT      GPIOB

/* Driver SPI control */

#define ISM_SPI_PORT        GPIOC

#define ISM_SPI_CLK_PIN     GPIO10
#define ISM_SPI_MISO_PIN    GPIO11
#define ISM_SPI_MOSI_PIN    GPIO12

#define ISM_SPI_CS_PIN      GPIO0
#define ISM_SPI_CS_PORT     GPIOE


static void set_gpios()
{
    rcc_periph_clock_enable(RCC_GPIOC);

    gpio_mode_setup(ISM_SPI_PORT,GPIO_MODE_AF,GPIO_PUPD_NONE,
                    ISM_SPI_CLK_PIN  | 
                    ISM_SPI_MOSI_PIN |
                    ISM_SPI_MISO_PIN);

    gpio_set_af(ISM_SPI_PORT,GPIO_AF6,
                    ISM_SPI_CLK_PIN  | 
                    ISM_SPI_MOSI_PIN |
                    ISM_SPI_MISO_PIN);
}

void initialize()
{
    rcc_periph_clock_enable(RCC_SPI3);
    set_gpios();
    spi_set_master_mode(SPI3);
    spi_init_master(SPI3,SPI_CR1_BAUDRATE_FPCLK_DIV_2,
                    SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                    SPI_CR1_CPHA_CLK_TRANSITION_1,
                    SPI_CR1_MSBFIRST);
    spi_enable(SPI3);
    
    spi_send(SPI3,0xdead);
}