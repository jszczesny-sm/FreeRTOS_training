
#include <serial.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <semphr.h>

static xSemaphoreHandle xSemaphore = NULL;

int _write(int fd, char *ptr, int len);

void serial_setup()
{
    rcc_periph_clock_enable(RCC_USART1);
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO6);
    gpio_set_af(GPIOB, GPIO_AF7,GPIO6);

	/* Setup UART parameters. */
	usart_set_baudrate(USART1, 115200);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_mode(USART1, USART_MODE_TX);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

	/* Finally enable the USART. */
	usart_enable(USART1);

    /* And lets create Mutex */
    xSemaphore = xSemaphoreCreateMutex();
}

int _write(int fd, char *ptr, int len)
{
	int i = 0;
    if (xSemaphoreTake(xSemaphore,portMAX_DELAY) == pdTRUE) {
        /*
        * Write "len" of char from "ptr" to file id "fd"
        * Return number of char written.
        *
        * Only work for STDOUT, STDIN, and STDERR
        */
        if (fd > 2) {
            return -1;
        }
        while (*ptr && (i < len)) {
            usart_send_blocking(USART1, *ptr);
            if (*ptr == '\n') {
                usart_send_blocking(USART1, '\r');
            }
            i++;
            ptr++;
        }
        xSemaphoreGive(xSemaphore);
    }
	return i;
}