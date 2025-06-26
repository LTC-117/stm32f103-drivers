#include "stm32f103xb.h"
#include "rcc.h"
#include "pwm_dma.h"
#include <stdint.h>


int main(void) {
    /* INIT PWM at PA0 AND PA1 */
//    tim_pwm_init();
    tim2_pwm_dma_init();

    /* Test 1 */
    /* tim2_pwm_set_dutycycle(1, 0); */
    /* tim2_pwm_set_dutycycle(2, 100); */

    while(1) {
//        for(int i = 0; i < 100; i++) {
//            tim2_pwm_set_dutycycle(2, i);
//            tim2_pwm_set_dutycycle(2, 100 - i);
//            for(int i = 0; i < 7500000; i++);
//        }

//        for(int i = 0; i < 100; i++) {
//            tim2_pwm_set_dutycycle(2, 100 - i);
//            tim2_pwm_set_dutycycle(2, i);
//            for(int i = 0; i < 7500000; i++);
//        }
    }

}


