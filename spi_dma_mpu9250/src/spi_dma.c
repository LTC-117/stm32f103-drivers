#include "spi_dma.h"
#include "stm32f103xb.h"


/*
 * NSS ->  PA4
 * SCK ->  PA5  ->  AF Push-Pull
 * MISO -> PA6  ->  Input Pull-Up
 * MOSI -> PA7  ->  AF Push-Pull
 */


#define SPI1_SCK            (9U << 20)
#define SPI1_MISO           (8U << 24)
#define SPI1_MOSI           (9U << 28)
#define SPI1_BR_4           (0x1U << 3)


void spi1_dma_init(void) {

    /* --------------- GPIO Configuration --------------- */

    /* Enable clock access to SPI1 Pins Port (A) */
    RCC->APB2ENR    |=      RCC_APB2ENR_IOPAEN;

    /* Set SPI pins modes */
    GPIOA->CRL      &=~     (0xFFF << 20);
    GPIOA->CRL      |=      SPI1_SCK;   // PA5
    GPIOA->CRL      |=      SPI1_MISO;  // PA6
    GPIOA->CRL      |=      SPI1_MOSI;  // PA7

    /* --------------- SPI Configuration --------------- */

    /* Enable clock access to SPI1 Module */
    RCC->APB2ENR    |=      RCC_APB2ENR_SPI1EN;

    /* Set Software Slave Management */
    SPI1->CR1       |=      SPI_CR1_SSM;
    SPI1->CR1       |=      SPI_CR1_SSI;

    /* Set the SPI1 as Master */
    SPI1->CR1       |=      SPI_CR1_MSTR;

    /* Set CPHA and CPOL */
    SPI1->CR1       |=      SPI_CR1_CPHA;
    SPI1->CR1       |=      SPI_CR1_CPOL;

    /* Set clock divider: PCLK / 4 */
    SPI1->CR1       |=      SPI1_BR_4;

    /* Select DMA Mode for the SPI Module */
    SPI1->CR2       |=      SPI_CR2_TXDMAEN;
    SPI1->CR2       |=      SPI_CR2_RXDMAEN;

    /* Enable SPI Module */
    SPI1->CR1       |=      SPI_CR1_SPE;

}


// DMA1 Channel 3
void spi1_dma_tx_init(void) {

    /* ------------------------- DMA Configuration ------------------------- */

    /* Enable clock access to DMA module */
    RCC->AHBENR    |=      RCC_AHBENR_DMA1EN;

    /* Disable DMA Channel  ->  F4 */
    /* Wait until DMA Channel is disabled  ->  F4 */

    /* >>>>> Configure DMA Parameters <<<<< */

    /* 1 - Enable Memory Address Increment (MINC) */
    DMA1_Channel3->CCR  |=  DMA_CCR_MINC;

    /* 2 - Set Transfer Direction: Memory to Peripheral */
    DMA1_Channel3->CCR  |=  DMA_CCR_DIR;

    /* 3 - Enable Transfer Complete Interrupt */
    DMA1_Channel3->CCR  |=  DMA_CCR_TCIE;

    /* 4 - Enable Transfer Error Interrupt */
    DMA1_Channel3->CCR  |=  DMA_CCR_TEIE;

    /* 5 - Enable DMA interrupt in the NVIC */
    NVIC_EnableIRQ(DMA1_Channel3_IRQn);

}


// DMA1 Channel 2
void spi1_dma_rx_init(void) {

    /* ------------------------- DMA Configuration ------------------------- */

    /* Enable clock access to DMA module */
    RCC->AHBENR    |=      RCC_AHBENR_DMA1EN;

    /* Disable DMA Channel  ->  F4 */
    /* Wait until DMA Channel is disabled  ->  F4 */

    /* >>>>> Configure DMA Parameters <<<<< */

    /* 1 - Enable Memory Address Increment (MINC) */
    DMA1_Channel2->CCR  |=  DMA_CCR_MINC;

    /* 2 - Set Transfer Direction: Peripheral to Memory */
    DMA1_Channel2->CCR  &= ~DMA_CCR_DIR;

    /* 3 - Enable Transfer Complete Interrupt */
    DMA1_Channel2->CCR  |=  DMA_CCR_TCIE;

    /* 4 - Enable Transfer Error Interrupt */
    DMA1_Channel2->CCR  |=  DMA_CCR_TEIE;

    /* 5 - Enable DMA interrupt in the NVIC */
    NVIC_EnableIRQ(DMA1_Channel2_IRQn);

}


void spi1_dma_transfer(uint32_t msg_to_snd, uint32_t msg_len) {
    /* Clear interrupt flags */
    DMA1->IFCR      |=      DMA_IFCR_CGIF3;

    /* Set Peripheral Address */
    DMA1_Channel3->CPAR  =  (uint32_t)(&(SPI1->DR));

    /* Set Memory Address */
    DMA1_Channel3->CMAR  =  msg_to_snd;

    /* Set transfer length */
    DMA1_Channel3->CNDTR =  msg_len;

    /* Enable the DMA Channel */
    DMA1_Channel3->CCR   =  DMA_CCR_EN;

}


void spi1_dma_receive(uint32_t received_msg, uint32_t msg_len) {
    /* Clear interrupt flags */
    DMA1->IFCR      |=      DMA_IFCR_CGIF2;

    /* Set Peripheral Address */
    DMA1_Channel2->CPAR  =  (uint32_t)(&(SPI1->DR));

    /* Set Memory Address */
    DMA1_Channel2->CMAR  =  received_msg;

    /* Set transfer length */
    DMA1_Channel2->CNDTR =  msg_len;

    /* Enable the DMA Channel */
    DMA1_Channel2->CCR   =  DMA_CCR_EN;

}


