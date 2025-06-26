#ifndef DMA_H_
#define DMA_H_


#include "stm32f103xb.h"
#include <stdint.h>


void dma1_mm_init(void);
void dma1_transfer_start(uint32_t src_buff, uint32_t dest_buff, uint32_t lenght);


#endif
