#include "stm32f103xb.h"
#include "rcc.h"
#include "adc_dma.h"
#include "usart_dma.h"
#include <stdint.h>
#include <string.h>


#define BUFFER_SIZE         5


extern uint16_t adc_raw_data[NUM_OF_CHANNELS];
char usart_data_buffer[USART_DATA_BUFF_SIZE];
uint8_t g_rx_complete;
uint8_t g_usart_complete;
char init_msg[] = "Hello, world";


int main(void) {
    rcc_init();
    usart2_rx_tx_init();
    dma1_init();
    dma1_rx_config(usart_data_buffer);
    dma1_tx_config((uint32_t)init_msg, strlen(init_msg));

    while(!g_usart_complete) {}

    while(1) {
    }

}


void DMA1_Channel6_IRQHandler(void) {
    if((DMA1->ISR)  &  DMA_ISR_TCIF6) {
        g_rx_complete = 1;

        /* Do something*/

        /* Clear the flag */
        DMA1->IFCR  |=  DMA_IFCR_CTCIF6;
    }
}


void DMA1_Channel7_IRQHandler(void) {
    if((DMA1->ISR)  &  DMA_ISR_TCIF7) {
        /* Do something */

        /* Clear the flag */
        DMA1->IFCR  |=  DMA_IFCR_CTCIF7;
    }
}


void USART2_IRQHandler(void) {
    g_usart_complete = 1;

    /* Clear TC Interrupt flag */
    USART2->SR      |=      USART_SR_TC;

}


