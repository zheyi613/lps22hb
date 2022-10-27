/**
 * @file lps22hb.c
 * @author zheyi613 (zheyi880613@gmail.com)
 * @brief LPS22HB library
 * @date 2022-10-24
 */
#include "lps22hb.h"

/* Message transmit interface */
#define MSG(str, len) CDC_Transmit_FS(str, len)

/* LPS22HB static variable */
static bool is_bypass = false;

/* User serial interface */
static inline int lps_read_reg(uint8_t reg, uint8_t *buff, uint8_t len)
{
	if (!is_bypass) {
		return HAL_I2C_Mem_Read(&LPS22HB_I2C, LPS22HB_ADDR << 1, reg,
					I2C_MEMADD_SIZE_8BIT, buff, len, 10);
	} else {
		int result = 0;
		for (uint8_t i = 0; i < len; i++) {
			result = HAL_I2C_Mem_Read(&LPS22HB_I2C, LPS22HB_ADDR << 1,
					      reg + i, I2C_MEMADD_SIZE_8BIT,
					      buff + i, 1, 10);
			if (result)
				return result;
		}
		return result;
	}
}

static inline int lps_write_reg(uint8_t reg, uint8_t *buff, uint8_t len)
{
	if (!is_bypass) {
		return HAL_I2C_Mem_Write(&LPS22HB_I2C, LPS22HB_ADDR << 1, reg,
					 I2C_MEMADD_SIZE_8BIT, buff, len, 10);
	} else {
		int result = 0;
		for (uint8_t i = 0; i < len; i++) {
			result = HAL_I2C_Mem_Write(&LPS22HB_I2C, LPS22HB_ADDR << 1,
					       reg + i, I2C_MEMADD_SIZE_8BIT,
					       buff + i, 1, 10);
			if (result)
				return result;
		}
		return result;
	}
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

static inline int lps22hb_get_whoami(uint8_t *whoami)
{
	return lps_read_reg(LPS22HB_WHO_AM_I, whoami, 1);
}

static inline int lps22hb_srst(void)
{
	uint8_t regval = 0x04;

	return lps_write_reg(LPS22HB_CTRL_REG2, &regval, 1);
}

/**
 * @brief Set threshold value for interrupt generation
 *
 * @param ths (hPa), min unit = 1/16 hPa
 * @return int
 */
static inline int lps22hb_set_press_threshold(float ths)
{
	int16_t regval = (int16_t)(ths * 16);

	return lps_write_reg(LPS22HB_THS_P_L, (uint8_t *)&regval, 2);
}

/**
 * @brief Set reference pressure
 *
 * @param ref (hPa), min unit = 1/4096 hPa
 * @return int
 */
static inline int lps22hb_set_ref_press(float ref)
{
	int32_t regval = (int32_t)(ref * 4096);

	return lps_write_reg(LPS22HB_REF_P_XL, (uint8_t *)regval, 3);
}

/**
 * @brief Set pressure offset
 *
 * @param ofs (hPa), min unit = 1/4096 hPa
 * @return int
 */
static int lps22hb_set_press_ofs(float ofs)
{
	int16_t regval = (int16_t)(ofs * 16);

	return lps_write_reg(LPS22HB_RPDS_L, (uint8_t *)&regval, 2);
}

int lps22hb_init(struct lps22hb_cfg lps22hb)
{
	int result;
	char str[80];
	uint8_t slen;
	uint8_t regval = 0;

	slen = snprintf(str, sizeof(str), "Initializing LPS22HB...%s", "\n\r");
	MSG((uint8_t *)str, slen);

	result = lps22hb_get_whoami(&regval);
	if (regval != LPS22HB_ID)
		return result;

	/* Soft reset */
	result = lps22hb_srst();
	if (result)
		return result;

	result = lps22hb_set_press_ofs(LPS22HB_PRESSURE_OFS);
	if (result)
		return result;
	
	result = lps22hb_set_ref_press(lps22hb.ref_press);
	if (result)
		return result;

	/* Set FIFO mode */
	regval = lps22hb.mode << 5;
	result = lps_write_reg(LPS22HB_FIFO_CTRL, &regval, 1);
	if (result)
		return result;

	/* Set ODR, LPF, bypass */
	if (lps22hb.mode == LPS22HB_BYPASS_MODE)
		is_bypass = true;
	regval = lps22hb.odr << 4 | lps22hb.lpf << 2 | is_bypass << 1;
	result = lps_write_reg(LPS22HB_CTRL_REG1, &regval, 1);
	if (result)
		return result;

	/* Reset LPF */
	result = lps_write_reg(LPS22HB_LPFP_RES, &regval, 1);
	if (result)
		return result;

	/*
	 * bypass mode: disable fifo and register address automatically
	 * incremented during a multiple byte access whith a serial interface.
	 * stream mode: otherwise
	 */
	regval = !is_bypass << 6 | !is_bypass << 4;
	result = lps_write_reg(LPS22HB_CTRL_REG2, &regval, 1);
	if (result)
		return result;

	slen = snprintf(str, sizeof(str), "Initialize successfully !%s",
			"\n\r");
	MSG((uint8_t *)str, slen);

	return 0;
}

static inline bool lps22hb_is_data_rdy(void)
{
	uint8_t status = 0;

	lps_read_reg(LPS22HB_STATUS, &status, 1);
	return ((status & 0x03) == 0x03) ? true : false;
}

static inline const int lps22hb_get_fifo_cnt(void)
{
	uint8_t status = 0;

	lps_read_reg(LPS22HB_FIFO_STATUS, &status, 1);
	return status & 0x3F;
}

#pragma pack(1)
typedef union {
	uint8_t bytes[5];
	struct {
		uint8_t press[3];
		int16_t temp;
	} data;
} lps22hb_data_t;
#pragma pack()

void lps22hb_read_bypass(float *press, float *temp)
{
	int result;
	lps22hb_data_t get_data;
	int32_t intpress = 0;

	if (!lps22hb_is_data_rdy())
		return;

	result = lps_read_reg(LPS22HB_PRESS_OUT_XL, get_data.bytes, 5);
	if (result)
		return;

	if (get_data.data.press[2] & 0x80) /* minus case */
		intpress = 0xFF000000UL;
	intpress |= get_data.data.press[2] << 16 | get_data.data.press[1] << 8 |
		    get_data.data.press[0];
	*press = (float)intpress / 4096;
	*temp = (float)get_data.data.temp / 100;
}

void lps22hb_read_stream(float *press, float *temp)
{
	const int cnt = lps22hb_get_fifo_cnt();
	if (cnt == 0)
		return;
	int result;
	lps22hb_data_t get_data[cnt];
	uint32_t intpress = 0;
	float press_total = 0;
	float temp_total = 0;

	result = lps_read_reg(LPS22HB_PRESS_OUT_XL, get_data[0].bytes, cnt * 5);
	if (result)
		return;
	
	for (int i = 0; i < cnt; i++) {
		intpress = 0;
		if (get_data[i].data.press[2] & 0x80) /* minus case */
		intpress = 0xFF000000UL;
		intpress |= get_data[i].data.press[2] << 16 | get_data[i].data.press[1] << 8 |
		    get_data[i].data.press[0];
		press_total += (float)intpress / 4096;
		temp_total += (float)get_data[i].data.temp / 100;
	}
	
	*press = press_total / cnt;
	*temp = temp_total / cnt;
}

void lps22hb_read_data(float *press, float *temp)
{
	if (is_bypass)
		lps22hb_read_bypass(press, temp);
	else
		lps22hb_read_stream(press, temp);
}



