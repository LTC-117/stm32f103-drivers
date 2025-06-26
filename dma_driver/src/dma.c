#include "dma.h"
#include "stm32f103xb.h"


void dma1_mm_init(void) {
    /* Enable Clock access to the DMA module */
    RCC->AHBENR         |=  RCC_AHBENR_DMA1EN;

    /* Disable DMA stream (-> F4 only) */
    DMA1_Channel4->CCR  &= ~DMA_CCR_EN;

    /* Wait until stream is disabled (-> F4 only) */

    /* Configure DMA parameters */

    /* Set MSIZE (Memory Data Size) to half-word */
    DMA1_Channel4->CCR  &= ~DMA_CCR_MSIZE_Msk;
    DMA1_Channel4->CCR  |=  DMA_CCR_MSIZE_0;

    /* Set PSIZE (Peripheral Data Size) to half-word */
    DMA1_Channel4->CCR  &= ~DMA_CCR_PSIZE_Msk;
    DMA1_Channel4->CCR  |=  DMA_CCR_PSIZE_0;

    /* Enable Peripheral and Memory Increment */
    DMA1_Channel4->CCR  |=  DMA_CCR_PINC;
    DMA1_Channel4->CCR  |=  DMA_CCR_MINC;

    /* Select MEM2MEM mode */
    DMA1_Channel4->CCR  |=  DMA_CCR_DIR;
    DMA1_Channel4->CCR  |=  DMA_CCR_MEM2MEM;

    /* Enable Transfer Complete Interrupt */
    DMA1_Channel4->CCR  |=  DMA_CCR_TCIE;

    /* Enable Transfer Error Interrupt */
    DMA1_Channel4->CCR  |=  DMA_CCR_TEIE;

    /* Disable direct mode (-> F4 only) */

    /* Set DMA FIFO threshold (-> F4 only) */

    /* Enable DMA interrupt in NVIC */
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);

}


void dma1_transfer_start(uint32_t src_buff, uint32_t dest_buff, uint32_t lenght) {
    /* Set peripheral address */
    DMA1_Channel4->CPAR     =   src_buff;

    /* Set memory address */
    DMA1_Channel4->CPAR     =   dest_buff;

    /* Set transfer lenght */
    DMA1_Channel4->CNDTR    =   lenght;

    /* Enable the DMA module */
    DMA1_Channel4->CCR      |=  DMA_CCR_EN;

}


