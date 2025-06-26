#include "stm32f103xb.h"
#include "rcc.h"
#include "iwdg.h"
#include "led.h"
#include "gpio_exti.h"
#include <stdint.h>


#define IWDG_KEY_RELOAD             0x0000AAAAU


uint8_t g_btn_press;


static void check_reset_source(void);


int main(void) {
    rcc_init();
    gpio_interrupt_init();
    led_init();
    check_reset_source();
    iwdg_init();

    while(1) {
        if(g_btn_press != 1) {
            /* Refresh IWDG down-counter to default value */
            IWDG->KR    =   IWDG_KEY_RELOAD;
        led_blink();
        }
    }

}


static void check_reset_source(void) {
    if((RCC->CSR  &  RCC_CSR_IWDGRSTF)  ==  (RCC_CSR_IWDGRSTF)) {
        /* Clear IWDG Reset Flag */
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


