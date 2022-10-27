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
static bool is_bdu = false;

/* User serial interface */
static inline int lps_read_reg(uint8_t reg, uint8_t *buff, uint8_t len)
{
	if (!is_bdu) {
		return HAL_I2C_Mem_Read(&LPS22HB_I2C, LPS22HB_ADDR << 1, reg,
					I2C_MEMADD_SIZE_8BIT, buff, len, 10);
	} else {
		int rc;
		for (uint8_t i = 0; i < len; i++) {
			rc = HAL_I2C_Mem_Read(&LPS22HB_I2C, LPS22HB_ADDR << 1,
					      reg + i, I2C_MEMADD_SIZE_8BIT,
					      buff + i, 1, 10);
			if (rc)
				return rc;
		}
		return rc;
	}
}

static inline int lps_write_reg(uint8_t reg, uint8_t *buff, uint8_t len)
{
	if (!is_bdu) {
		return HAL_I2C_Mem_Write(&LPS22HB_I2C, LPS22HB_ADDR << 1, reg,
					 I2C_MEMADD_SIZE_8BIT, buff, len, 10);
	} else {
		int rc;
		for (uint8_t i = 0; i < len; i++) {
			rc = HAL_I2C_Mem_Write(&LPS22HB_I2C, LPS22HB_ADDR << 1,
					       reg + i, I2C_MEMADD_SIZE_8BIT,
					       buff + i, 1, 10);
			if (rc)
				return rc;
		}
		return rc;
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

static int lps22hb_get_whoami(uint8_t *whoami)
{
	return lps_read_reg(LPS22HB_WHO_AM_I, whoami, 1);
}

static int lps22hb_srst(void)
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
static int lps22hb_set_press_threshold(float ths)
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
static int lps22hb_set_ref_press(float ref)
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
	int16_t regval = (int16_t)(ofs * 4096);

	return lps_write_reg(LPS22HB_RPDS_L, (uint8_t *)&regval, 2);
}

int lps22hb_init(struct lps22hb_cfg lps22hb)
{
	int rc;
	char str[80];
	uint8_t slen;
	uint8_t regval = 0;

	slen = snprintf(str, sizeof(str), "Initializing LPS22HB...%s", "\n\r");
	MSG((uint8_t *)str, slen);

	rc = lps22hb_get_whoami(&regval);
	if (regval != LPS22HB_ID)
		return rc;

	/* Soft reset */
	rc = lps22hb_srst();
	if (rc)
		return rc;

	/* Set ODR, LPF, BDU */
	regval = lps22hb.odr | lps22hb.lpf | lps22hb.is_bdu << 1;
	rc = lps_write_reg(LPS22HB_CTRL_REG1, &regval, 1);
	if (rc)
		return rc;
	is_bdu = lps22hb.is_bdu;

	/* Disable register address automatically incremented */
	if (lps22hb.is_bdu) {
		regval = 0;
		rc = lps_write_reg(LPS22HB_CTRL_REG2, &regval, 1);
		if (rc)
			return rc;
	}

	slen = snprintf(str, sizeof(str), "Initialize successfully !%s",
			"\n\r");
	MSG((uint8_t *)str, slen);

	return 0;
}

int lps22hb_init2(struct lps22hb_cfg lps22hb)
{
	int rc;
	char str[80];
	uint8_t slen;
	uint8_t regval = 0;

	slen = snprintf(str, sizeof(str), "Initializing LPS22HB...%s", "\n\r");
	MSG((uint8_t *)str, slen);

	rc = lps22hb_get_whoami(&regval);
	if (regval != LPS22HB_ID)
		return rc;

	/* Soft reset */
	rc = lps22hb_srst();
	if (rc)
		return rc;

	/* Set ODR, LPF, BDU */
	regval = lps22hb.odr << 4 | lps22hb.lpf << 2 | lps22hb.is_bdu << 1;
	rc = lps_write_reg(LPS22HB_CTRL_REG1, &regval, 1);
	if (rc)
		return rc;
	is_bdu = lps22hb.is_bdu;

	/* Reset LPF */
	rc = lps_read_reg(LPS22HB_LPFP_RES, &regval, 1);
	if (rc)
		return rc;

	regval = 0x02 << 5;
	rc = lps_write_reg(LPS22HB_FIFO_CTRL, &regval, 1);
	if (rc)
		return rc;
	regval = 0x10 | 0x40;
	rc = lps_write_reg(LPS22HB_CTRL_REG2, &regval, 1);
	if (rc)
		return rc;
	slen = snprintf(str, sizeof(str), "Initialize successfully !%s",
			"\n\r");
	MSG((uint8_t *)str, slen);

	return 0;
}




static bool lps22hb_is_data_rdy(void)
{
	uint8_t status = 0;

	lps_read_reg(LPS22HB_STATUS, &status, 1);
	return ((status & 0x03) == 0x03) ? true : false;
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

void lps22hb_read_data(float *press, float *temp)
{
	int rc;
	lps22hb_data_t get_data;
	int32_t intpress = 0;

	if (!lps22hb_is_data_rdy())
		return;

	rc = lps_read_reg(LPS22HB_PRESS_OUT_XL, get_data.bytes, 5);
	if (rc)
		return;

	if (get_data.data.press[2] & 0x80) /* minus case */
		intpress = 0xFF000000UL;
	intpress |= get_data.data.press[2] << 16 | get_data.data.press[1] << 8 |
		    get_data.data.press[0];
	*press = (float)intpress / 4096;
	*temp = (float)get_data.data.temp / 100;
}

void lps22hb_read_press(float *press)
{
	int rc;
	uint8_t raw_press[3] = {0};
	int32_t intpress = 0;

	if (!lps22hb_is_data_rdy())
		return;

	rc = lps_read_reg(LPS22HB_PRESS_OUT_XL, raw_press, 3);
	if (rc)
		return;

	if (raw_press[2] & 0x80) /* minus case */
		intpress = 0xFF000000UL;
	intpress |= raw_press[2] << 16 | raw_press[1] << 8 | raw_press[0];
	*press = (float)intpress / 4096.0;
}

void lps22hb_read_temp(float *temp)
{
	int rc;
	int16_t raw_temp;

	if (!lps22hb_is_data_rdy())
		return;

	rc = lps_read_reg(LPS22HB_PRESS_OUT_XL, (uint8_t *)&raw_temp, 3);
	if (rc)
		return;

	*temp = (float)raw_temp / 100.0;
}

static const int lps22hb_get_fifo_cnt(void)
{
	uint8_t status = 0;

	lps_read_reg(LPS22HB_FIFO_STATUS, &status, 1);
	return status & 0x3F;
}

void lps22hb_read_fifo(float *press, float *temp)
{
	int rc = 0;
	const int cnt = lps22hb_get_fifo_cnt();
	if (cnt == 0)	return;
	lps22hb_data_t get_data[cnt];
	uint32_t intpress = 0;
	float press_total = 0;
	float temp_total = 0;

	rc = lps_read_reg(LPS22HB_PRESS_OUT_XL, get_data[0].bytes, cnt * 5);
	if (rc)
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