#include "stm32f103xb.h"
#include "rcc.h"
#include "adc_dma.h"


#define BUFFER_SIZE         5


extern uint16_t adc_raw_data[NUM_OF_CHANNELS];


int main(void) {
    rcc_init();
    adc_dma_init();

    while(1) {}

}


