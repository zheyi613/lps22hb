/**
 * @file lps22hb.c
 * @author zheyi613 (zheyi880613@gmail.com)
 * @brief LPS22HB library
 * @date 2022-10-24
 */
#include "lps22hb.h"

/* User serial interface define */
static inline int lps_read_reg(uint8_t reg, uint8_t *buff, uint8_t len)
{
	return HAL_I2C_Mem_Read(&LPS22HB_I2C, LPS22HB_ADDR << 1, reg,
				I2C_MEMADD_SIZE_8BIT, buff, len, 100);
}

static inline int lps_write_reg(uint8_t reg, uint8_t *buff, uint8_t len)
{
	return HAL_I2C_Mem_Write(&LPS22HB_I2C, LPS22HB_ADDR << 1, reg,
				 I2C_MEMADD_SIZE_8BIT, buff, len, 100);
}

enum lps22hb_reg {
	LPS22HB_INTERRUPT_CFG = 0x0B,
	LPS22HB_THS_P_L,
	LPS22HB_THS_P_H,
	LPS22HB_WHO_AM_I = 0x0F,
	LPS22HB_CTRL_REG1,
	LPS22HB_CTRL_REG2,
	LPS22HB_CTRL_REG3,
	LPS22HB_FIFO_CTRL = 0x14,
	LPS22HB_REF_P_XL,
	LPS22HB_REF_P_L,
	LPS22HB_REF_P_H,
	LPS22HB_RPDS_L,
	LPS22HB_RPDS_H,
	LPS22HB_RES_CONF,
	LPS22HB_INT_SOURCE = 0x25,
	LPS22HB_FIFO_STATUS,
	LPS22HB_STATUS,
	LPS22HB_PRESS_OUT_XL,
	LPS22HB_PRESS_OUT_L,
	LPS22HB_PRESS_OUT_H,
	LPS22HB_TEMP_OUT_L,
	LPS22HB_TEMP_OUT_H,
	LPS22HB_LPFP_RES = 0x33
};

static int lps22hb_get_whoami(uint8_t *whoami)
{
        return lps_read_reg(LPS22HB_WHO_AM_I, whoami, 1);
}

int lps22hb_init(void)
{
        int rc;
        uint8_t regval = 0;

        // rc = lps22hb_get_whoami(&regval);
        rc = lps22hb_get_whoami(&regval);
        if (regval != LPS22HB_ID)
                return rc;

        return regval;
}
