#include "stm32f103xb.h"
#include "rcc.h"
#include "led.h"
#include "gpio_exti.h"
#include "wwdg.h"
#include <stdint.h>


#define IWDG_KEY_RELOAD             0x0000AAAAU


uint8_t g_btn_press;


static void check_reset_source(void);
void clock_config(void);


int main(void) {
    rcc_init();
    clock_config();
    gpio_interrupt_init();
    led_init();
    check_reset_source();
    wwdg_init();

    while(1) {
        if(g_btn_press != 1) {
            /* Set counter */
            WWDG->CR    &=  ~WWDG_CR_T;
            WWDG->CR    |=  0x7E;

            led_blink();
        }
    }

}


static void check_reset_source(void) {
    if((RCC->CSR  &  RCC_CSR_WWDGRSTF)  ==  (RCC_CSR_WWDGRSTF)) {
        /* Clear WWDG Reset Flag */
        RCC->CSR    |=      RCC_CSR_RMVF;

        /* Turn LED On */
        led_on();

        while(g_btn_press != 1) {
        }

        g_btn_press = 0;
    }

}


static void btn_callback(void) {
    g_btn_press = 1;
}


void EXTI15_10_IRQHandler(void) {
    if((EXTI->PR  &  EXTI_IMR_IM14) == (EXTI_IMR_IM14)) {
        /* Clear EXTI Flag */
        EXTI->PR  =  EXTI_IMR_IM14;

        /* Do something... */
        btn_callback();
    }
}
/**
  * @brief  System Clock Configuration
  *         The System Clock is configured as follow:
  *             System Clock source             = HSE
  *             SYSCLK (Hz)                     = 8000000
  *             HCLK (Hz)                       = 2000000
  *             AHB Prescaler                   = 4
  *             APB1 Prescaler                  = 1
  *             APB2 Prescaler                  = 1
  *             HSE Frequency (Hz)              = 8000000
  *             VDD (V)                         = 3.3
  *             Main regulator output voltage   = Scale1 mode
  *             Flash Latency (WS)              = 0
  * @param  None
  * @retval None
  */

void clock_config(void) {
    /* Enable HSE Oscillator */
    RCC->CR     |=      RCC_CR_HSEBYP;
    RCC->CR     |=      RCC_CR_HSEON;

    /* Wait till HSERDY goes LOW */
    while((RCC->CR  &  RCC_CR_HSERDY) == (RCC_CR_HSERDY)) {}

    /* Set FLASH latency */
    FLASH->ACR  &=      ~FLASH_ACR_LATENCY;
    FLASH->ACR  |=      FLASH_ACR_LATENCY_0;

    /* Sysclk activation on the main PLL */
    RCC->CFGR   &=      ~RCC_CFGR_HPRE;
    RCC->CFGR   |=      RCC_CFGR_HPRE_DIV4;

    /* Set clock source */
    RCC->CFGR   &=      ~RCC_CFGR_SW;
    RCC->CFGR   |=      RCC_CFGR_SW_HSE;

    /* Wait til HSE is used as system clock */
    while((RCC->CFGR  &  RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE) {}

    /* Set APB1 Prescaler */
    RCC->CFGR   &=      ~RCC_CFGR_PPRE1;
    RCC->CFGR   |=      RCC_CFGR_PPRE1_DIV1;

    /* Set APB2 Prescaler */
    RCC->CFGR   &=      ~RCC_CFGR_PPRE2;
    RCC->CFGR   |=      RCC_CFGR_PPRE2_DIV1;

}


