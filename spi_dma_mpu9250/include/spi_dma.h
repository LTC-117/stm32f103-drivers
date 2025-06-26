#ifndef __SPI_DMA_H_
#define __SPI_DMA_H_


#include "stm32f103xb.h"
#include <stdint.h>


void spi1_dma_init(void);
void spi1_dma_tx_init(void);
void spi1_dma_rx_init(void);
void spi1_dma_transfer(uint32_t msg_to_snd, uint32_t msg_len);
void spi1_dma_receive(uint32_t received_msg, uint32_t msg_len);


#endif
