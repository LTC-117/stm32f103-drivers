#include "i2c_dma.h"
#include "stm32f103xb.h"
#include <stdint.h>
#include <stddef.h>

/* Pinout:
 * I2C1_SCL     ->   PB6:   Alternate Function Open Drain
 * I2C1_SDA     ->   PB7:   Alternate Function Open Drain
 *
 * I2C1_TX      ->   DMA_Channel7
 * I2C1_RX      ->   DMA_Channel6
 */


#define SCL_PB6             (9U << 24)
#define SDA_PB7             (9U << 28)
#define PERIPH_CLK          16

#define I2C_100KHZ          80  // 0b 0101 0000
#define MAX_RISETIME        17


void i2c1_init(void) {

    /* --------------- I2C GPIO Configuration --------------- */

    /* Enable clock access to GPIOB */
    RCC->APB2ENR    |=      RCC_APB2ENR_IOPBEN;

    /* Set PB6 and PB7 as Alternate Function Open Drain */
    GPIOB->CRL      |=      SCL_PB6;
    GPIOB->CRL      |=      SDA_PB7;

    /* --------------- I2C Configuration --------------- */

    /* Enable clock access to I2C1 */
    RCC->APB1ENR    |=      RCC_APB1ENR_I2C1EN;

    /* Reset I2C Module */
    I2C1->CR1       |=      I2C_CR1_SWRST;

    /* Release the reset */
    I2C1->CR1       &=     ~I2C_CR1_SWRST;

    /* Disable clock stretching */
    I2C1->CR1       &=     ~I2C_CR1_NOSTRETCH;

    /* Disable General call */
    I2C1->CR1       &=     ~I2C_CR1_ENGC;

    /* Select to use DMA */
    I2C1->CR2       |=      I2C_CR2_DMAEN;

    /* Set source clock speed */
    I2C1->CR2       |=      PERIPH_CLK;

    /* Set I2C to standard mode, 100KHz */
    I2C1->CCR       =       I2C_100KHZ; // Based on computation
    I2C1->TRISE     =       MAX_RISETIME;

    /* Enable the I2C Module */
    I2C1->CR1       |=      I2C_CR1_PE;

}


// Channel 7
void i2c1_tx_dma_init(void) {
    /* Enable clock access to DMA */
    RCC->AHBENR     |=      RCC_AHBENR_DMA1EN;

    /* Enable Mem Addr increment */
    DMA1_Channel7->CCR  |=  DMA_CCR_MINC;

    /* Enable Transfer Complete Interrupt */
    DMA1_Channel7->CCR  |=  DMA_CCR_TCIE;

    /* Set Transfer Direction */
    DMA1_Channel7->CCR  |=  DMA_CCR_DIR;

    /* Enable Channel 7 Interrupt in NVIC */
    NVIC_EnableIRQ(DMA1_Channel7_IRQn);

}


// Channel 6
void i2c1_rx_dma_init(void) {
    /* Enable clock access to DMA */
    RCC->AHBENR     |=      RCC_AHBENR_DMA1EN;

    /* Enable Mem Addr increment */
    DMA1_Channel6->CCR  |=  DMA_CCR_MINC;

    /* Enable Transfer Complete Interrupt */
    DMA1_Channel6->CCR  |=  DMA_CCR_TCIE;

    /* Set Transfer Direction: Periph to Mem */
    DMA1_Channel6->CCR  &=  ~DMA_CCR_DIR;

    /* Enable Channel 7 Interrupt in NVIC */
    NVIC_EnableIRQ(DMA1_Channel6_IRQn);
}


void i2c1_dma_transfer(uint8_t *msg_to_send, uint32_t msg_len) {
    if(NULL != msg_to_send) {
        /* Clear interrupt flags */
        DMA1->IFCR  |=      DMA_IFCR_CTCIF7;

        /* Set Peripheral Address */
        DMA1_Channel7->CPAR |=  (uint32_t)(&(I2C1->DR));

        /* Set Memory Address */
        DMA1_Channel7->CMAR |=  (uint32_t)msg_to_send;

        /* Set Transfer Length */
        DMA1_Channel7->CNDTR |= (uint32_t)msg_len;

        /* Enable the DMA Channel (7) */
        DMA1_Channel7->CCR  |=  DMA_CCR_EN;

    }

    else {
        // Do something ...
    }
}


void i2c1_dma_receive(uint8_t *received_msg, uint32_t msg_len) {
    if(NULL != received_msg) {
        /* Clear interrupt flags */
        DMA1->IFCR  |=      DMA_IFCR_CTCIF6;

        /* Set Peripheral Address */
        DMA1_Channel6->CPAR |=  (uint32_t)(&(I2C1->DR));

        /* Set Memory Address */
        DMA1_Channel6->CMAR |=  (uint32_t)received_msg;

        /* Set Transfer Length */
        DMA1_Channel6->CNDTR |= (uint32_t)msg_len;

        /* Enable the DMA Channel (6) */
        DMA1_Channel6->CCR  |=  DMA_CCR_EN;
    }

    else {
        // Do something ...
    }
}


void i2c_dma_read(uint8_t slave_addr, uint8_t register_addr, uint8_t *p_read_buff, uint16_t num_of_bytes) {
    /* Wait while BUSY flag is set */
    while(I2C1->SR2  &  I2C_SR2_BUSY) {}

    /* Generate START Condition */
    I2C1->CR1       |=      I2C_CR1_START;

    /* Wait for the SB flag to be set */
    while(!(I2C1->SR1  &  I2C_SR1_SB)) {}

    /* Read SR1 */
    I2C1->SR1;

    /* Send the Slave Address */
    I2C1->DR        |=      (slave_addr << 1 | 0);

    /* Wait for Address Flag to be set */
    while(!(I2C1->SR1  &  I2C_SR1_ADDR)) {}

    /* Read SR1 */
    I2C1->SR1;

    /* Read SR2 */
    I2C1->SR2;

    /* Wait for TXE flag to be set */
    while(!(I2C1->SR1  &  I2C_SR1_TXE)) {}

    /* Send Register Address */
    if(num_of_bytes >= 2) {
        /* Enable ACK */
        I2C1->CR1   |=  I2C_CR1_ACK;

        /* Send register address to read */
        I2C1->DR    =   register_addr;
    }
    else {
        /* Disable ACK */
        I2C1->CR1   &=  ~I2C_CR1_ACK;

        /* Send register address to read */
        I2C1->DR    =   register_addr;
    }

    /* Wait for BTF flag to be set */
    while(!(I2C1->SR1  &  I2C_SR1_BTF)) {}

    /* Generate RESTART */
    I2C1->CR1       |=      I2C_CR1_START;

    /* Wait for the SB flag to be set */
    while(!(I2C1->SR1  &  I2C_SR1_SB)) {}

    /* Read SR1 */
    I2C1->SR1;

    /* Send Slave Addr with read */
    I2C1->DR    =   (slave_addr << 1 | 1);

    /* Wait for the Address Flag to be set */
    while(!(I2C1->SR1  &  I2C_SR1_ADDR)) {}

    /* Call DMA Receive Function */
    i2c1_dma_receive(p_read_buff, num_of_bytes);

    /* Read SR1 */
    I2C1->SR1;

    /* Read SR2 */
    I2C1->SR2;

}


void i2c_dma_write(uint8_t slave_addr, uint8_t *p_write_buff, uint16_t num_of_bytes) {
    /* Wait while BUSY flag is set */
    while(I2C1->SR2  &  I2C_SR2_BUSY) {}

    /* Generate START condition */
    I2C1->CR1       |=      I2C_CR1_START;

    /* Wait for the SB flag to be set */
    while(!(I2C1->SR1  &  I2C_SR1_SB)) {}

    /* Read SR1 */
    I2C1->SR1;

    /* Send Slave Addr "Write" */
    I2C1->DR        |=      (slave_addr << 1 | 0);

    /* Wait for Addr Flag to be set */
    while(!(I2C1->SR1  &  I2C_SR1_ADDR)) {}

    /* Call DMA transfer function */
    i2c1_dma_transfer(p_write_buff, num_of_bytes);

    /* Read SR1 */
    I2C1->SR1;

    /* Read SR2 */
    I2C1->SR2;

}


void DMA1_Channel7_IRQHandler(void) {
    if((DMA1->ISR)  &  DMA_ISR_TCIF7) {
        /* do something */

        /* Clear the flag */
        DMA1->IFCR  |=      DMA_IFCR_CTCIF7;

    }
}


void DMA1_Channel6_IRQHandler(void) {
    if((DMA1->ISR)  &  DMA_ISR_TCIF7) {
        /* do something */

        /* Clear the flag */
        DMA1->IFCR  |=      DMA_IFCR_CTCIF6;

    }
}


