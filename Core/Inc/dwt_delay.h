/**
 * @file dwt_delay.h
 * @author zheyi613 (zheyi880613@gmail.com)
 * @brief Implement accuracy delay us / ms with DWT
 * @date 2022-10-20
 */

#ifndef __DWT_DELAY_H
#define __DWT_DELAY_H

#include "main.h"

void DWT_Init(void);
void DWT_Delay(uint32_t us);

#endif /* __DWT_DELAY_H */