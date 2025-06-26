#include "rcc.h"
#include "stm32f103xb.h"


void rcc_init(void) {
    RCC->CR         |=      RCC_CR_HSEON;
    while(!(RCC->CR   &  RCC_CR_HSERDY)) {}
    RCC->CFGR       |=      RCC_CFGR_SW_0;
}

