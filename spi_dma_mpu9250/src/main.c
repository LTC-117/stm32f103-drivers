#include "rcc.h"
#include "spi_dma.h"
#include "mpu9250.h"
#include <stdint.h>


/* Pin out
 *
 *  MPU ------------------> STM32
 *  VCC          -          3.3V
 *  GND          -          GND
 *  NCS          -          PA0
 *  SCL          -          SCK/PA5
 *  SDI/SDA      -          MOSI/PA7
 *  SDO/ADP      -          MISO/PA6
 */


float acc_x, acc_y, acc_z;


int main(void) {
    /* Enable system clock */
    rcc_init();

    /* Enable SPI */
    spi1_dma_init();

    /* Enable Floating Point Unit (FPU) -> (F4 Only) */

    /* Configure NCS Pin */
    mpu9250_ncs_pin_config();

    /* Enable TX */
    spi1_dma_tx_init();

    /* Enable RX */
    spi1_dma_rx_init();

    /* Reset NCS Pin */
    mpu9250_ncs_pin_reset();

    /* Config Accel */
    mpu9250_accel_config(ACC_FULL_SCALE_2_G);


    while(1) {
        /* Reset NCS Pin */
        mpu9250_ncs_pin_reset();

        /* Update Accel Values */
        mpu9250_accel_update();

        /* Get Accel Data */
        acc_x = mpu9250_get_x();
        acc_y = mpu9250_get_y();
        acc_z = mpu9250_get_z();

        /* Set NCS PIn */
        mpu9250_ncs_pin_set();

    }
}


