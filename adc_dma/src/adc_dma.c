#include "adc_dma.h"
#include "stm32f103xb.h"
#include <stdint.h>


/*
 * Projeto: Driver ADC com DMA
 *
 * Descrição:
 * Este projeto implementa um driver para leitura de sensores utilizando ADC (Conversor Analógico-Digital)
 * com transferência de dados via DMA (Acesso Direto à Memória).
 *
 * Periféricos envolvidos:
 * 1 - GPIO -> Pino onde o sensor está conectado.
 * 2 - ADC  -> Responsável por converter o sinal analógico do sensor em digital.
 * 3 - DMA  -> Transferência eficiente dos dados convertidos do ADC para a memória sem intervenção da CPU.
 */

// ADC1_IN0  ->  PA0
// ADC1_IN1  ->  PA1
// DMA1_ADC1 ->  Channel 1


#define PA0_ANALOG_INPUT            ~(0xFU << 0)
#define PA1_ANALOG_INPUT            ~(0xFU << 4)

uint16_t adc_raw_data[NUM_OF_CHANNELS];


void adc_dma_init(void) {

    /* ------------------------ GPIO Configuration ------------------------- */

    /* Enable clock access to ADC GPIO Pin's Port */
    RCC->APB2ENR    |=      RCC_APB2ENR_IOPAEN;

    /* Set PA0 and PA1 mode to input analog mode */
    GPIOA->CRL      &=      PA0_ANALOG_INPUT;
    GPIOA->CRL      &=      PA1_ANALOG_INPUT;

    /* ------------------------ ADC Configuration -------------------------- */

    /* Enable clock access to ADC Peripheral */
    RCC->APB2ENR    |=      RCC_APB2ENR_ADC1EN;

    /* Set sequence length (number of conversions) */
    ADC1->SQR1      &=     ~ADC_SQR1_L_Msk;
    ADC1->SQR1      |=      ADC_SQR1_L_0; // -> 2 Conversions (1U < 20)

    /* Set sequence (PA0 or PA1 first), depends on the specific SQ value: */
    // -> 00000 = First conversion
    // -> 00001 = Second conversion
    // -> 00010 = Third conversion
    // -> 00011 = Fourth conversion
    // ...
    ADC1->SQR3      |=      (0U << 0); // -> PA0 First
    ADC1->SQR3      |=      (1U << 5); // -> PA2 Second

    /* Enable scan mode */
    ADC1->CR1       |=      ADC_CR1_SCAN;

    /* Select to use DMA */
    ADC1->CR2       |=      ADC_CR2_DMA;

    /* Set Continuous Mode */
    ADC1->CR2       |=      ADC_CR2_CONT;

    /* ------------------------- DMA Configuration ------------------------- */

    /* Enable clock access to DMA module */
    RCC->AHBENR     |=      RCC_AHBENR_DMA1EN;

    /* Disable DMA Channel  ->  F4 */
    /* Wait until DMA Channel is disabled  ->  F4 */

    /* >>>>> Configure DMA Parameters <<<<< */

    /* 1 - Enable Circular Mode */
    DMA1_Channel1->CCR  |=  DMA_CCR_CIRC;

    /* 2 - Set Memory Transfer Size */
    DMA1_Channel1->CCR  |=  DMA_CCR_MSIZE_0; // -> Half-Word (16-bits)

    /* 3 - Set Peripheral Transfer Size */
    DMA1_Channel1->CCR  |=  DMA_CCR_PSIZE_0; // -> Half-Word (16-bits)

    /* 4 - Enable Memory Increment (MINC) */
    DMA1_Channel1->CCR  |=  DMA_CCR_MINC;

    /* 5 - Enable Peripheral Increment (PINC) */
    DMA1_Channel1->CCR  |=  DMA_CCR_PINC;

    /* 6 - Set Peripheral Address */
    DMA1_Channel1->CPAR =  (uint32_t)(&(ADC1->DR));

    /* 7 - Set Memory Address */
    DMA1_Channel1->CMAR =  (uint32_t)(&adc_raw_data);

    /* 8 - Set number of trasnfer */
    DMA1_Channel1->CNDTR = (uint16_t)NUM_OF_CHANNELS;

    /* 9 - Enable DMA Channel */
    DMA1_Channel1->CCR  |=  DMA_CCR_EN;

    /* ------------------------ ADC Initialization ------------------------- */

    /* Enable ADC */
    ADC1->CR2       |=      ADC_CR2_ADON;

    /* Start ADC */
    ADC1->CR2       |=      ADC_CR2_SWSTART;
}


