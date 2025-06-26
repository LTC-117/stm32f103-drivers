#include "led.h"


void led_init(void) {
    RCC->APB2ENR    |=      RCC_APB2ENR_IOPCEN;

    GPIOC->CRH      &=      ~(0xFU << 20);
    GPIOC->CRH      |=      (1U << 20);
}


void led_on(void) {
    GPIOC->BSRR     =       (1U << (13 + 16));
}


void led_off(void) {
    GPIOC->BSRR     =       (1U << 13);
}


void led_blink(void) {
    for(int i = 0; i < 200000; i++) {
        led_on();
    }

    for(int i = 0; i < 200000; i++) {
        led_off();
    }
}


