#ifndef _BSP_H_
#define _BSP_H_

// SYSTEM CLOCK SETTINGS 
#define PLL_MULTIPLIER        12
#define PLL_DIVISOR           1

#define SYSTEM_CORE_FREQUENCY 48e6
#define AHB_FREQUENCY         SYSTEM_CORE_FREQUENCY
#define APB1_FREQUENCY        AHB_FREQUENCY
#define APB2_FREQUENCY        AHB_FREQUENCY


void configure_system(void);


#endif