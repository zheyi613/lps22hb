/**
 * @file dwt_delay.c
 * @author zheyi613 (zheyi880613@gmail.com)
 * @brief Implement accuracy delay us / ms with DWT
 * @date 2022-10-20
 */

#include "dwt_delay.h"

void DWT_Init(void)
{
        // if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {
                CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
                // DWT->LAR = 0xC5ACCE55; /* Use in Core-M7 */
                DWT->CYCCNT = 0;
                DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
        // }

       
}

void DWT_Delay(uint32_t us)
{
        uint32_t startTick = DWT->CYCCNT,
                 delayTicks = us * (SystemCoreClock / 1000000);
        
        while(DWT->CYCCNT - startTick < delayTicks);
}