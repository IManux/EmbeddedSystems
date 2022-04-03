/*
 * stm32f4xx_i2c_basic.c
 *
 *  Created on: 30-12-2021
 *      Author: Acer
 *
 *  Reference:
 *  https://github.com/elliottt/stm32f4/blob/master/stm32f4/src/i2c.c
 *
 *  I2C Interrupts
 *  Event flag	Interrupt event										Enable control bit
 *  SB			Start bit sent (Master)									ITEVFEN
 *  ADDR		Address sent (Master) or Address matched (Slave)		ITEVFEN
 *  ADD10		10-bit header sent (Master)								ITEVFEN
 *  STOPF		Stop received (Slave)									ITEVFEN
 *  BTF			Data byte transfer finished								ITEVFEN
 *  RxNE		Receiver buffer not empty								ITEVFEN and ITBUFEN
 *  TxE			Transmit buffer empty									ITEVFEN and ITBUFEN
 *  BERR		Bus error												ITERREN
 *  ARLO		Arbitration Lost (Master)								ITERREN
 *  AF			Acknowledge failure										ITERREN
 *  OVR			Overrun/Underrun										ITERREN
 *  PECERR		PEC	error												ITERREN
 *  TIMEOUT		Timeout													ITERREN
 *  SMBALERT	SMBus Alert												ITERREN
 *
 *  I2C Registers
 *  - I2C_CR1
 *  - I2C_CR2
 *  - I2C_OAR1
 *  - I2C_OAR2
 *	- I2C_DR
 *	- I2C_SR1
 *	- I2C_SR2
 *	- I2C_CCR
 *	- I2C_TRISE
 *	- I2C_FLTR
 *
 */
#include <stddef.h>
#include <stdlib.h>

#include <stm32f4xx.h>
#include <stm32f4xx_ll_utils.h>
#include <stm32f4xx_ll_i2c.h>
/*
#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include <task.h>
*/
#include "stm32f4xx_i2c_basic.h"

#define I2CX_HW_TIMEOUT		 1000
#define I2CX_MASTER_W			0	// Mode Master Write
#define I2CX_MASTER_R			1	// Mode Master Read
#define I2CX_MASTER_WR			2	// Mode Write & Read

#define I2CX_SET_START (I2CX) \
					do {	\
						I2CX->hw->CR1 |= I2C_CR1_START; \
						while (i2c->hw->CR1 & I2C_CR1_START); }	\
					while (0)

#define I2CX_SET_STOP  (I2CX)	\
					do {	\
						I2CX->hw->CR1 |= I2C_CR1_STOP; \
						while (i2c->hw->CR1 & I2C_CR1_STOP); }	\
					while (0)

struct i2cx_hw_t {
	I2C_TypeDef* hw;
	volatile uint8_t mode;		// Master Write or Master Read
	volatile uint8_t addr;
	volatile uint8_t reg;
	volatile uint8_t err;
	volatile uint32_t tout;
	volatile uint8_t len;		// len of buff
	volatile uint8_t* buff;		// buff

	volatile uint8_t state;
	//reserved to add RTOS
};

struct i2cx_hw_t _i2c1_hw_drv = {	I2C1, 0, 0, 0, 0, 1000, NULL, 0	};
struct i2cx_hw_t _i2c2_hw_drv = {	I2C2, 0, 0, 0, 0, 1000, NULL, 0	};
struct i2cx_hw_t _i2c3_hw_drv = {	I2C3, 0, 0, 0, 0, 1000, NULL, 0	};


static inline void __i2c_set_start(struct i2cx_hw_t *i2c)
{
	 i2c->hw->CR1 |= I2C_CR1_START;
	 while (i2c->hw->CR1 & I2C_CR1_START);
}

static inline void __i2c_set_stop(struct i2cx_hw_t *i2c)
{
	i2c->hw->CR1 |= I2C_CR1_STOP;
	while (i2c->hw->CR1 & I2C_CR1_STOP);
}

uint8_t i2c_init(struct i2cx_hw_t *i2c)
{
	return 0;
}

uint8_t i2c_execute(struct i2cx_hw_t *i2c, uint8_t addr, uint8_t reg, uint8_t *buff, uint8_t len)
{
	uint8_t err;
	uint32_t tout;

	i2c->reg = reg;
	i2c->buff = buff;
	i2c->len = len;
	i2c->err = 0;
	i2c->state = 1;

	err = 0;
	tout = i2c->tout;

	/*	timeout */
	while (i2c->state) {
		if (!tout) {
			break;
		}
		LL_mDelay(0);	// 1 ms
		tout--;
	};

	if (i2c->state) {	//process not finished?
		//LL_I2C_GenerateStopCondition(i2c->hw);
		__i2c_set_stop(i2c);
	}

	i2c->state = 0;
	err = i2c->err;

	return err;
}

uint8_t i2c_write_reg(struct i2cx_hw_t *i2c, uint8_t addr, uint8_t reg, uint8_t *buff, uint8_t len)
{
	uint8_t err;

	i2c->mode = I2CX_MASTER_W;
	i2c->addr = (addr << 1);
	err = i2c_execute(i2c, addr, reg, buff, len);

	return err;
}

uint8_t i2c_read_reg(struct i2cx_hw_t *i2c, uint8_t addr, uint8_t reg, uint8_t *buff, uint8_t len)
{
	uint8_t err;
	uint8_t *arr;

	err = 1;
	len++;
	arr = (uint8_t*) malloc(len);
	if (arr == NULL) {
		return err;
	}

	arr[0] = reg;
	i2c->mode = I2CX_MASTER_R;
	i2c->addr = (addr << 1) | 0x01;
	err = i2c_execute(i2c, addr, reg, arr, len);

	buff = &arr[1];
	free(arr);

	return err;
}

/*	FUNCTION PRIVATE TO HANDLE INTERRUPT */
static void __i2c_event_irq_handler(struct i2cx_hw_t *i2c)
{
/* 7-bit Master Transmitter - Sequence
 *
 * [S]	   [ADDRESS][A]			   [Data1][A][Data2][A]		....[DataN][A]		 [P]
 *	  (EV5)			   (EV6)(EV8_1)(EV8)	 (EV8)	   (EV8)			  (EV8_2)
 *
 * Legend: S=Start, SR=Repeated start, P=stop, A=Acknowledge (ACK)
 * EVX: Event (if ITEVFEN=1)
 * EV5: SB=1, cleared by reading SR1 register followed by writing DR register with address.
 * EV6: ADDR=1, cleared by reading SR1 register followed by reading SR2.
 * EV8_1: TxE=1, shift register empty, data register empty, write Data1 in DR.
 * EV8: TxE=1, shift register not empty, data register empty, cleared by writing DR register.
 * EV_2: TxE=1, BTF=1, Program stop request, TxE and BTF are cleared by hardware by the stop condition.
 * EV9: ADD10=1, cleared by reading SR1 register followed by writing DR register.
 * */

    /* Read both status registers */
    uint32_t sr1 = i2c->hw->SR1;
    uint32_t cr1 = i2c->hw->CR1;
    i2c->hw->SR2;

    /* EV Start bit sent */
    if (sr1 & LL_I2C_SR1_SB) {
    	i2c->hw->DR = i2c->addr;
    }

    /*	address sent */
    if (sr1 & LL_I2C_SR1_ADDR) {	// EV6
    	if (i2c->mode == I2CX_MASTER_W || i2c->mode == I2CX_MASTER_WR) {
    		if (i2c->len) {
    			i2c->hw->DR = *(i2c->buff);
    			i2c->buff++;
    			i2c->len--;
    		} else {
    			LL_I2C_GenerateStopCondition(i2c->hw);
    			i2c->state = 0;
    		}
    		if (i2c->mode == I2CX_MASTER_WR) {
    			i2c->mode = I2CX_MASTER_R;
    		}
    	} else { // I2CX_MASTER_R
    		if (i2c->len > 1) {
    			/* Send ACK on next read byte */
    			i2c->hw->CR1 |= LL_I2C_ACK;
    		} else {
    			LL_I2C_GenerateStopCondition(i2c->hw);
    		}
    	}
    }

    /*	RX not empty - new byte arrived */
    if (sr1 & LL_I2C_SR1_RXNE) {
    	if (i2c->mode == I2CX_MASTER_R) {
    		*(i2c->buff) = i2c->hw->DR;
    		i2c->buff++;
    		i2c->len--;

    		if (i2c->len == 1) {
    			/* Unset Ack, set Stop */
    			i2c->hw->CR1 &= ~I2C_CR1_ACK;
    			__i2c_set_stop(i2c);
    		}

    		if (i2c->len == 0) {
    			i2c->state = 0;
    		}
    	}
    }

    /* TXE set, BTF clear: tx buffer empty, still writing. */
    if (sr1 & I2C_SR1_TXE && !(sr1 & I2C_SR1_BTF)) {
        if (i2c->mode == I2CX_MASTER_W) {
        	if (i2c->len) {
            /* send next byte from write buffer. */
        		i2c->hw->DR = *(i2c->buff);
        		i2c->buff++;
        		i2c->len--;
        	} else {
                __i2c_set_stop(i2c);
                i2c->state = 0;		// process finished
        	}
        } else {
            if (i2c->len) {
                /* done writing, now reading: send repeated stat */
                __i2c_set_start(i2c);
            } else {
                /* done reading: send stop */
                __i2c_set_stop(i2c);
                i2c->state = 0;
            }
        }
    }

}

static void __i2c_error_irq_handler(struct i2cx_hw_t *i2c)
{
	/* Read SRs to clear them */
	i2c->hw->SR1;
	i2c->hw->SR2;

	/* Write 0 to SR1, to clear */
	i2c->hw->SR1 = 0;

	/* Send Stop */
	LL_I2C_GenerateStopCondition(i2c->hw);

	/*	Control Bit: ITERREN
	 *  Error Bits
	 * 	BERR: Bus error.
	 * 	ARLO: Arbitration lost (Master).
	 * 	AF: Acknowledge failure.
	 * 	OVR: Overrun/Underrun.
	 * 	PECERR: PEC error.
	 * 	TIMEOUT: Timeout error.
	 *  SMBALERT: SMBus Alert.
	 * */
	i2c->err = 1;
	i2c->state = 0;
}

/*	ISR	I2C EV	*/
void I2C1_EV_IRQHandler (void)
{
    __i2c_event_irq_handler(i2c1);
}

void I2C2_EV_IRQHandler (void) {
    __i2c_event_irq_handler(i2c2);
}

void I2C3_EV_IRQHandler (void) {
    __i2c_event_irq_handler(i2c3);
}

/*	ISR I2C ERR	*/
void I2C1_ER_IRQHandler (void)
{
    __i2c_error_irq_handler(i2c1);
}

void I2C2_ER_IRQHandler (void) {
    __i2c_error_irq_handler(i2c2);
}

void I2C3_ER_IRQHandler (void) {
    __i2c_error_irq_handler(i2c3);
}
