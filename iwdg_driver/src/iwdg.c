#include "iwdg.h"
#include <stdint.h>


#define IWDG_KEY_ENABLE             0x0000CCCCU
#define IWDG_KEY_WR_ACCESS_ENABLE   0x00005555U
#define IWDG_PRESCALER_4            0x00000000U
#define IWDG_RELOAD_VAL             0xFFF
#define IWDG_KEY_RELOAD             0x0000AAAAU
#define READ_BIT(REG, BIT)          ((REG) & (BIT))


static uint8_t is_iwdg_ready(void);


void iwdg_init(void) {
    /* Enable the IWDG by writing 0x0000CCCC in the IWDG_KR register */
    IWDG->KR        =       IWDG_KEY_ENABLE;

    /* Enable Register access by writing 0x00005555 in the IWDG_KR register */
    IWDG->KR        =       IWDG_KEY_WR_ACCESS_ENABLE;

    /* Set the IWDG Prescaler */
    IWDG->PR        =       IWDG_PRESCALER_4;

    /* Set the reload register (IWDG_RLR) to the largest value 0xFFF */
    IWDG->RLR       =       IWDG_RELOAD_VAL;

    /* Wait for the registers to be updated (IWDG_SR = 0x00000000) */
    while(is_iwdg_ready() != 1) {}

    /* Refresh the counter value with IWDG_KR (IWDG_KR = 0x0000AAAA) */
    IWDG->KR        =       IWDG_KEY_RELOAD;

}


static uint8_t is_iwdg_ready(void) {
    return ((READ_BIT(IWDG->SR, IWDG_SR_PVU  |  IWDG_SR_RVU) == 0U) ? 1UL : 0UL);
}


