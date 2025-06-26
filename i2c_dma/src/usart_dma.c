#include "usart_dma.h"
#include "adc_dma.h"
#include "stm32f103xb.h"
#include <stdint.h>


// USART2 TX -> PA2
// USART2 RX -> PA3


#define PA2_PUSH_PULL           (9U << 8)
#define PA3_INPUT_FLOAT         (4 << 12)
#define CLK                     8000000
#define BAUDRATE                9600


static uint16_t compute_usart_br(uint32_t periph_clk, uint32_t baudrate) {
    return ((periph_clk + (baudrate / 2U)) / baudrate);
}

static void usart_set_baudrate(uint32_t periph_clk, uint32_t baudrate) {
    USART2->BRR     |=      compute_usart_br(periph_clk, baudrate);
}


void usart2_rx_tx_init(void) {

    /* --------------- Configure UART GPIO Pin --------------- */

    /* 1. Enable clock access to GPIOA */
    RCC->APB2ENR    |=      RCC_APB2ENR_IOPAEN;

    /* 2. Set PA2 (TX) mode to Alternate Function Push-pull */
    GPIOA->CRL      |=      PA2_PUSH_PULL;

    /* 3. Set PA3 (RX) mode to Input Floating/Pull-up */
    GPIOA->CRL      |=      PA3_INPUT_FLOAT;


    /* --------------- Configure USART Module --------------- */

    /* 1. Enable clock access to USART2 Module */
    RCC->APB1ENR    |=      RCC_APB1ENR_USART2EN;

    /* 2. Set Baudrate */
    usart_set_baudrate(CLK, BAUDRATE);

    /* 3. Select to use DMA */
    USART2->CR3     |=      USART_CR3_DMAT;
    USART2->CR3     |=      USART_CR3_DMAR;

    /* 4. Set transfer direction */
    USART2->CR1     |=      USART_CR1_TE;
    USART2->CR1     |=      USART_CR1_RE;

    /* 5. Clear TC (Transfer Complete) Flag */
    USART2->SR      &=     ~USART_SR_TC;

    /* 6. Enable USART Transfer Complete Interrupt */
    USART2->CR1     |=      USART_CR1_TCIE;

    /* 7. Enable USART Module */
    USART2->CR1     |=      USART_CR1_UE;

}


// USART2_RX DMA -> Channel 6
// USART2_TX DMA -> Channel 7


void dma1_init(void) {
    /* Enable clock access to DMA1 */
    RCC->AHBENR     |=      RCC_AHBENR_DMA1EN;

    /* Enable DMA1 Channel 7 Interrupt */
    NVIC_EnableIRQ(DMA1_Channel7_IRQn);
}


void dma1_rx_config(char *data_buffer) {
    /* Disable DMA stream (F4 Only) */

    /* Wait till DMA is disabled (F4 Only) */

    /* Clear interrupt flags for Channel 6 */
    DMA1->IFCR      |=      DMA_IFCR_CGIF6;

    /* Set peripheral address */
    DMA1_Channel6->CPAR =   (uint32_t)(&(USART2->DR));

    /* Set memory address */
    DMA1_Channel6->CMAR =   (uint32_t)(&data_buffer);

    /* Set number of transfers */
    DMA1_Channel6->CNDTR =  (uint16_t)USART_DATA_BUFF_SIZE;

    /* Enable memory address increment */
    DMA1_Channel6->CCR  |=  DMA_CCR_MINC;

    /* Enable transfer complete interrupt */
    DMA1_Channel6->CCR  |=  DMA_CCR_TCIE;

    /* Enable Circular Mode */
    DMA1_Channel6->CCR  |=  DMA_CCR_CIRC;

    /* Set transfer direction: Peripheral to Memory */
    DMA1_Channel6->CCR  &=  ~DMA_CCR_DIR;

    /* Enable DMA Channel 6 */
    DMA1_Channel6->CCR  |=  DMA_CCR_EN;

    /* Enable DMA Channel 6 Interrupt in NVIC */
    NVIC_EnableIRQ(DMA1_Channel6_IRQn);

}


void dma1_tx_config(uint32_t msg_to_snd, uint32_t msg_len) {
    /* Disable DMA stream (F4 Only) */

    /* Wait till DMA is disabled (F4 Only) */

    /* Clear interrupt flags for Channel 7 */
    DMA1->IFCR      |=      DMA_IFCR_CGIF7;

    /* Set peripheral address */
    DMA1_Channel7->CPAR =   (uint32_t)(&(USART2->DR));

    /* Set memory address */
    DMA1_Channel7->CMAR =   msg_to_snd;

    /* Set number of transfers */
    DMA1_Channel7->CNDTR =  msg_len;

    /* Enable memory address increment */
    DMA1_Channel7->CCR  |=  DMA_CCR_MINC;

    /* Enable trasnfer complete interrupt */
    DMA1_Channel7->CCR  |=  DMA_CCR_TCIE;

    /* Enable Circular Mode */
    DMA1_Channel7->CCR  |=  DMA_CCR_CIRC;

    /* Set transfer direction: Memory to Peripheral */
    DMA1_Channel7->CCR  |=  DMA_CCR_DIR;

    /* Enable DMA Channel 7 */
    DMA1_Channel7->CCR  |=  DMA_CCR_EN;

}


