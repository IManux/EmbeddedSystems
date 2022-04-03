/*
 * stm32f4xx_external_basic.h
 *
 *  Created on: Apr 2, 2022
 *      Author: ingma
 */

#ifndef INC_STM32F4XX_EXTERNAL_BASIC_H_
#define INC_STM32F4XX_EXTERNAL_BASIC_H_

uint32_t external_set_line(uint32_t ext, uint32_t port, uint8_t spull, uint8_t trigger);
void external_init();
void external_enable_interrupt(uint32_t ext);
void external_disable_interrupt(uint32_t ext);
void external_set_callback(uint32_t ext, void(*extcb)(void));



#endif /* INC_STM32F4XX_EXTERNAL_BASIC_H_ */
