#include "stm32f103xb.h"
#include "rcc.h"
#include "dma.h"


#define BUFFER_SIZE         5


uint32_t sensor_data[BUFFER_SIZE] = {892, 731, 1234, 90, 23};
uint32_t temp_data_arr[BUFFER_SIZE];
volatile uint8_t g_transfer_complete;


int main(void) {
    rcc_init();
    dma1_mm_init();

    g_transfer_complete = 0;

    for(int i = 0; i < BUFFER_SIZE; i++) {
        dma1_transfer_start(sensor_data[i], temp_data_arr[i], BUFFER_SIZE);
    }

    /* Wait until transfer complete */
    while(!g_transfer_complete) {}

    while(1) {}

}


void DMA1_Channel4_IRQHandler(void) {
    /* Check if transfer complete interrupt occured */
    if(DMA1->ISR  &  DMA_ISR_TCIF4) {
        g_transfer_complete = 1;

        /* Clear flag */
        DMA1->IFCR  |=  DMA_IFCR_CTCIF4;
    }

    /* Check if transfer error occurred */
    if(DMA1->ISR  &  DMA_ISR_TEIF4) {
        /* Do something.... */
        DMA1->IFCR  |=  DMA_IFCR_CTEIF4;
    }
}


