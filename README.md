# lps22hb
LPS22HB library test on STM32F401
- [LPS22HB datasheet](https://www.st.com/resource/en/datasheet/dm00140895.pdf)

## User Guide

1. Include your header files, define serial interface and pin SA0 in lps22hb.h
```c=11
#include "i2c.h"
#include "usbd_cdc_if.h"
#include <stdbool.h>

#define LPS22HB_I2C     hi2c1     
#define LPS22HB_SA0     0
```
2. Define your serial interface (ex: uart...) to transmit message
```c=9
/* Message transmit interface */
#define MSG(str, len) CDC_Transmit_FS(str, len)
```
3. Modify the read/write function to your i2c funtion
```c=15
/* User serial interface */
static inline int lps22hb_read_reg(uint8_t reg, uint8_t *buff, uint8_t len)
{
	return HAL_I2C_Mem_Read(&LPS22HB_I2C, LPS22HB_ADDR << 1, reg,
				I2C_MEMADD_SIZE_8BIT, buff, len, 10);
}

static inline int lps22hb_write_reg(uint8_t reg, uint8_t *buff, uint8_t len)
{
	return HAL_I2C_Mem_Write(&LPS22HB_I2C, LPS22HB_ADDR << 1, reg,
				 I2C_MEMADD_SIZE_8BIT, buff, len, 10);
}
```
4. See example code in main.c

Note: You can modify LPS22HB_PRESSURE_OFS according to your sensor offset in lps22hb.h.
```c=20
#define LPS22HB_PRESSURE_OFS    2.74 /* hPa */
```

