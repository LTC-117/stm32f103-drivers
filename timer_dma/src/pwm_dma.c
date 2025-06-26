#include "pwm_dma.h"
#include "stm32f103xb.h"


#define LOOKUP_TABLE_LEN        200


uint32_t ch1_dutycycle_lookup[LOOKUP_TABLE_LEN] = {
    0, 50, 100, 151, 201, 250, 300, 349, 398, 446, 494, 542, 589, 635, 681, 726, 771, 814,
    857, 899, 940, 981, 1020, 1058, 1095, 1131, 1166, 1200, 1233, 1264, 1294, 1323, 1351, 1377,
    1402, 1426, 1448, 1468, 1488, 1505, 1522, 1536, 1550, 1561, 1572, 1580, 1587, 1593, 1597, 1599,
    1600, 1599, 1597, 1593, 1587, 1580, 1572, 1561, 1550, 1536, 1522, 1505, 1488, 1468, 1448, 1426, 1402,
    1377, 1351, 1323, 1294, 1264, 1233, 1200, 1166, 1131, 1095, 1058, 1020, 981, 940, 899, 857, 814,
    771, 726, 681, 635, 589, 542, 494, 446, 398, 349, 300, 250, 201, 151, 100, 50, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


uint32_t ch2_dutycycle_lookup[LOOKUP_TABLE_LEN] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 50, 100, 151, 201, 250, 300, 349, 398, 446, 494, 542, 589, 635, 681, 726, 771,
    814, 857, 899, 940, 981, 1020, 1058, 1095, 1131, 1166, 1200, 1233, 1264, 1294, 1323, 1351, 1377,
    1402, 1426, 1448, 1468, 1488, 1505, 1522, 1536, 1550, 1561, 1572, 1580, 1587, 1593, 1597, 1599, 1600,
    1599, 1597, 1593, 1587, 1580, 1572, 1561, 1550, 1536, 1522, 1505, 1488, 1468, 1448, 1426, 1402,
    1377, 1351, 1323, 1294, 1264, 1233, 1200, 1166, 1131, 1095, 1058, 1020, 981, 940, 899, 857,
    814, 771, 726, 681, 635, 589, 542, 494, 446, 398, 349, 300, 250, 201, 151, 100, 50, 0
};

void tim2_pwm_dma_init(void) {

    /* --------------- GPIO Configuration --------------- */

    /* Enable clock access to GPIOA */
    RCC->APB2ENR    |=      RCC_APB2ENR_IOPAEN;

    /* Set PA0 and PA1 mode to output AF */

    /* PA0 */
    GPIOA->CRL      &=      ~(0xFU << 0);
    GPIOA->CRL      |=      (9U << 0);

    /* PA1 */
    GPIOA->CRL      &=      ~(0xFU << 4);
    GPIOA->CRL      |=      (9U << 4);

    /* --------------- TIM2 Configuration --------------- */

    /* Enable clock access to TIM2 */
    RCC->APB1ENR    |=      RCC_APB1ENR_TIM2EN;

    /* Set timer Prescaler */
    TIM2->PSC       =       0;

    /* Set auto-reload value */
    TIM2->ARR       =       1600 - 1;

    /* Reset the timer count */
    TIM2->CNT       =       0;

    /* Set PA1 (CH2) and PA0 (CH1) to 110 : PWM mode 1 */

    /* PA0 */
    TIM2->CCMR1     &=      ~(7U << 4);
    TIM2->CCMR1     |=      (6U << 4);

    /* PA1 */
    TIM2->CCMR1     &=      ~(7U << 12);
    TIM2->CCMR1     |=      (6U << 12);

    /* Configure DMA Interrupt Enable Register */
    TIM2->DIER      |=      TIM_DIER_CC1DE;
    TIM2->DIER      |=      TIM_DIER_CC2DE;
    TIM2->DIER      |=      TIM_DIER_UDE;
    TIM2->DIER      |=      TIM_DIER_TDE;

    /* Enable CH1 and CH2 */
    TIM2->CCER      |=      TIM_CCER_CC1E;
    TIM2->CCER      |=      TIM_CCER_CC2E;

    /* --------------- Configure DMA Parameters -------------- */
    // TIM2_CH1 -> DMA1_Channel5
    // TIM2_CH2 -> DMA1_Channel7

    /* Enable clock access to DMA1 */
    RCC->AHBENR    |=      RCC_AHBENR_DMA1EN;

    /********** TIM2_CH1 (DMA1_Channel5) Configuration **********/

    /* Enable Circular Mode */
    DMA1_Channel7->CCR  |=  DMA_CCR_CIRC;

    /* Enable Mem Addr increment */
    DMA1_Channel5->CCR  |=  DMA_CCR_MINC;

    /* Set Mem Size: 32-bit */
    DMA1_Channel5->CCR  &=  ~(3U << 13);
    DMA1_Channel5->CCR  |=  (1U << 14);

    /* Set Periph size: 32-bit */
    DMA1_Channel5->CCR  &=  ~(3U << 11);
    DMA1_Channel5->CCR  |=  (1U << 12);

    /* Set Transfer direction: Mem to Periph */
    DMA1_Channel5->CCR  &=  ~(3U << 6);
    DMA1_Channel5->CCR  |=  (1U << 6);

    /* Set number of transfer */
    DMA1_Channel5->CNDTR |= (uint16_t)LOOKUP_TABLE_LEN;

    /* Set peripheral address */
    DMA1_Channel5->CPAR  |= (uint32_t)(&TIM2->CCR1);

    /* Set memory address */
    DMA1_Channel5->CMAR  =  (uint32_t)(&ch1_dutycycle_lookup);

    /* Enable DMA Channel 5 */
    DMA1_Channel5->CCR  |=  DMA_CCR_EN;

    /********** TIM2_CH2 (DMA1_Channel7) Configuration **********/

    /* Enable Circular Mode */
    DMA1_Channel7->CCR  |=  DMA_CCR_CIRC;

    /* Enable Mem Addr increment */
    DMA1_Channel7->CCR  |=  DMA_CCR_MINC;

    /* Set Mem Size: 32-bit */
    DMA1_Channel7->CCR  &=  ~(3U << 13);
    DMA1_Channel7->CCR  |=  (1U << 14);

    /* Set Periph size: 32-bit */
    DMA1_Channel7->CCR  &=  ~(3U << 11);
    DMA1_Channel7->CCR  |=  (1U << 12);

    /* Set Transfer direction: Mem to Periph */
    DMA1_Channel7->CCR  &=  ~(3U << 6);
    DMA1_Channel7->CCR  |=  (1U << 6);

    /* Set number of transfer */
    DMA1_Channel7->CNDTR |= (uint16_t)LOOKUP_TABLE_LEN;

    /* Set peripheral address */
    DMA1_Channel7->CPAR  |= (uint32_t)(&TIM2->CCR2);

    /* Set memory address */
    DMA1_Channel7->CMAR  =  (uint32_t)(&ch2_dutycycle_lookup);

    /* Enable DMA Channel 5 */
    DMA1_Channel7->CCR  |=  DMA_CCR_EN;

    /* Enable timer */
    TIM2->CR1       |=      TIM_CR1_CEN;

}


