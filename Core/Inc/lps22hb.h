/**
 * @file lps22hb.h
 * @author zheyi613 (zheyi880613@gmail.com)
 * @brief LPS22HB library
 * @date 2022-10-24
 */

#ifndef LPS22HB_H
#define LPS22HB_H

#include "i2c.h"
#include <stdbool.h>

#define LPS22HB_I2C     hi2c1     
#define LPS22HB_SA0     0
#define LPS22HB_ADDR    (0b1011100 | LPS22HB_SA0) 
#define LPS22HB_ID      0xB1

enum lps22hb_odr {
        LPS22HB_POWER_DOWN,
        LPS22HB_1HZ,
        LPS22HB_10HZ,
        LPS22HB_25HZ,
        LPS22HB_50HZ,
        LPS22HB_75HZ
};

int lps22hb_init(void);

#endif