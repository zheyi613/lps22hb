/**
 * @file lps22hb.h
 * @author zheyi613 (zheyi880613@gmail.com)
 * @brief LPS22HB library
 * @date 2022-10-24
 */

#ifndef LPS22HB_H
#define LPS22HB_H

#include "i2c.h"
#include "usbd_cdc_if.h"
#include <stdbool.h>

#define LPS22HB_I2C     hi2c1     
#define LPS22HB_SA0     0
#define LPS22HB_ADDR    (0b1011100 | LPS22HB_SA0) 
#define LPS22HB_ID      0xB1

enum lps22hb_odr {
        LPS22HB_POWER_DOWN = 0,
        LPS22HB_1HZ = 16,
        LPS22HB_10HZ,
        LPS22HB_25HZ,
        LPS22HB_50HZ,
        LPS22HB_75HZ
};

enum lps22hb_lpf {
        LPS22HB_BW_ODR_DIV_2 = 0,
        LPS22HB_BW_ODR_DIV_9 = 8,
        LPS22HB_BW_ODR_DIV_20
};

/*
 * is_spi: 1: spi / 0: i2c
 * is_bdu: 1: output register not updated until MSB and LSB have been read
 *         0: continuous update
 * odr:    output data rate refer to enum lps22hb_odr
 * lpf:    low pass filter refer to enum lps22hb_lpf
 */
struct lps22hb_cfg {
        bool is_bdu;
        enum lps22hb_odr odr;
        enum lps22hb_lpf lpf;
};


int lps22hb_init(struct lps22hb_cfg lps22hb);
void lps22hb_read_data(float *press, float *temp);
void lps22hb_read_press(float *press);
void lps22hb_read_temp(float *temp);

#endif