#ifndef USART_DMA_H_
#define USART_DMA_H_


#include "stm32f103xb.h"


#define USART_DATA_BUFF_SIZE    5


void usart2_rx_tx_init(void);
void dma1_init(void);
void dma1_rx_config(char *data_buffer);
void dma1_tx_config(uint32_t msg_to_snd, uint32_t msg_len);


#endif
