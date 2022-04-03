/*
 * stm32f4xx_i2c_basic.h
 *
 *  Created on: 30-12-2021
 *      Author: Acer
 */

#ifndef INC_STM32F4XX_I2C_BASIC_H_
#define INC_STM32F4XX_I2C_BASIC_H_

#include <stdint.h>

#define I2CX_NO_ERR		0
#define I2CX_INIT_ERR	1
#define I2CX_TOUT_ERR	2
#define I2CX_BUFF_ERR	3
#define I2CX_ADDR_ERR	4
#define I2CX_DATA_ERR	5
#define I2CX_COMU_ERR	6


#ifdef __cplusplus
extern "C" {
#endif


struct i2cx_hw_t;

uint8_t i2c_init(struct i2cx_hw_t *i2c);
uint8_t i2c_execute(struct i2cx_hw_t *i2c, uint8_t addr, uint8_t reg, uint8_t *buff, uint8_t len);
uint8_t i2c_write_reg(struct i2cx_hw_t *i2c, uint8_t addr, uint8_t reg, uint8_t *buff, uint8_t len);
uint8_t i2c_read_reg(struct i2cx_hw_t *i2c, uint8_t addr, uint8_t reg, uint8_t *buff, uint8_t len);

extern struct i2cx_hw_t _i2c1_hw_drv;
#define i2c1 (&_i2c1_hw_drv)

extern struct i2cx_hw_t _i2c2_hw_drv;
#define i2c2 (&_i2c2_hw_drv)

extern struct i2cx_hw_t _i2c3_hw_drv;
#define i2c3 (&_i2c3_hw_drv)

#ifdef __cplusplus
}
#endif

#endif /* INC_STM32F4XX_I2C_BASIC_H_ */
