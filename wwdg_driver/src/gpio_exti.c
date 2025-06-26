#include "gpio_exti.h"
#include "stm32f103xb.h"


void gpio_interrupt_init(void) {
    /* Enable clock access for GPIOC */
    RCC->APB2ENR    |=      RCC_APB2ENR_IOPCEN;

    /* Set PC14 as input */
    GPIOC->CRH      &=      ~(0xFU << 24);
    GPIOC->CRH      |=      (4U << 24);

    /* Enable clock access to SYSCFG */
    RCC->APB2ENR    |=      RCC_APB2ENR_AFIOEN;

    /* Select PORTC for EXTI14 */
    AFIO->EXTICR[3] &=      ~(0xFU << 8);
    AFIO->EXTICR[3] |=      (2U << 8);

    /* Unmask EXTI14 */
    EXTI->IMR       |=      (1U << 14);

    /* Select falling edge trigger */
    EXTI->FTSR      |=      (1U << 14);

    /* Enable EXTI14 line in NVIC */
    NVIC_EnableIRQ(EXTI15_10_IRQn);

}


