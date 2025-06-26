#include "mpu9250.h"
#include "spi_dma.h"
#include <stdint.h>


#define SPI_DATA_BUFF_LEN       2
#define USER_CTRL_I2C_IF_DIS    (1U << 4)
#define MAX_TRANSFER_LEN        6
#define READ_FLAG               0x80


uint8_t dummy_buff[MAX_TRANSFER_LEN + 1];
uint8_t accel_buff[MAX_TRANSFER_LEN + 1];

double g_accel_range;
uint8_t spi_data_buff[SPI_DATA_BUFF_LEN];
uint8_t g_tx_cmplt;
uint8_t g_rx_cmplt;


void mpu9250_ncs_pin_config(void) {
    /* Enable clock access to GPIOA */
    RCC->AHBENR     |=      RCC_APB2ENR_IOPAEN;

    /* Set PA0 as Output Alternate Function Push-Pull */
    GPIOA->CRL      |=      (9U << 0);
}


void mpu9250_ncs_pin_set(void) {
    GPIOA->ODR      |=      (1U << 0);
}


void mpu9250_ncs_pin_reset(void) {
    GPIOA->ODR      &=      ~(1U << 0);
}


void mpu9250_accel_config(uint8_t mode) {
    switch(mode) {
        case ACC_FULL_SCALE_2_G:
            g_accel_range = 2.0;
            break;
        case ACC_FULL_SCALE_4_G:
            g_accel_range = 4.0;
            break;
        case ACC_FULL_SCALE_8_G:
            g_accel_range = 8.0;
            break;
        case ACC_FULL_SCALE_16_G:
            g_accel_range = 16.0;
            break;
        default:
            break;
    }

    /* Set to SPI mode only */
    spi_data_buff[0] = MPU_ADDR_USER_CTRL;
    spi_data_buff[1] = USER_CTRL_I2C_IF_DIS;

    spi1_dma_transfer((uint32_t)spi_data_buff, (uint32_t)SPI_DATA_BUFF_LEN);

    /* Wait for transfer completion */
    while(!g_tx_cmplt) {}

    /* Reset flag */
    g_tx_cmplt = 0;

    /* Configure the ACCEL Range */
    spi_data_buff[0] = MPU_ADDR_ACCELCONFIG;
    spi_data_buff[1] = mode;

    spi1_dma_transfer((uint32_t)spi_data_buff, (uint32_t)SPI_DATA_BUFF_LEN);

    /* Wait for transfer completion */
    while(!g_tx_cmplt) {}

    /* Reset flag */
    g_tx_cmplt = 0;
}


void mpu9250_accel_update(void) {
    dummy_buff[0] = MPU_ACCEL_XOUT_H | READ_FLAG;

    spi1_dma_receive((uint32_t)accel_buff, (uint32_t)(MAX_TRANSFER_LEN + 1)); 

    spi1_dma_transfer((uint32_t)dummy_buff, (uint32_t)(MAX_TRANSFER_LEN + 1));

    /* Wait for reception completion */
    while(!g_rx_cmplt) {}

    /* Reset flag */
    g_rx_cmplt = 0;
}


float mpu9250_accel_get(uint8_t high_idx, uint8_t low_idx) {
    int16_t result;

    result = (accel_buff[high_idx] << 8)  |  accel_buff[low_idx];

    if(result) {
        return ((float)-result) * g_accel_range / (float)0x8000;
    }
    else {
        return 0.0;
    }
}


float mpu9250_get_x(void) {
    return mpu9250_accel_get(1, 2);
}


float mpu9250_get_y(void) {
    return mpu9250_accel_get(3, 4);
}


float mpu9250_get_z(void) {
    return mpu9250_accel_get(5, 6);
}


void DMA1_Channel2_IRQHandler(void) {
    if((DMA1->ISR)  &  DMA_ISR_TCIF2) {
        // Do something...
        g_tx_cmplt = 1;

        // Clear the flag
        DMA1->IFCR  |=      DMA_IFCR_CTCIF2;
    }
    else if((DMA1->ISR)  &  DMA_ISR_TEIF2) {
        // Do something...

        // Clear the flag
        DMA1->IFCR  |=      DMA_IFCR_CTEIF2;
    }
}


void DMA1_Channel3_IRQHandler(void) {
    if((DMA1->ISR)  &  DMA_ISR_TCIF3) {
        // Do something...
        g_rx_cmplt = 1;

        // Clear the flag
        DMA1->IFCR  |=      DMA_IFCR_CTCIF3;
    }
    else if((DMA1->ISR)  &  DMA_ISR_TEIF3) {
        // Do something...

        // Clear the flag
        DMA1->IFCR  |=      DMA_IFCR_CTEIF3;
    }
}


