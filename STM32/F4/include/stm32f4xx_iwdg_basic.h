/*
 * stm32f4xx_iwdg_basic.h
 *
 *  Created on: 25-12-2021
 *      Author: Acer
 */

#ifndef INC_STM32F4XX_IWDG_BASIC_H_
#define INC_STM32F4XX_IWDG_BASIC_H_

#include "stm32f4xx_ll_iwdg.h"

#ifdef __cplusplus
extern "C" {
#endif

uint32_t iwdg_init(uint32_t ms);
void iwdg_reset(void);


#ifdef __cplusplus
}
#endif


#endif /* INC_STM32F4XX_IWDG_BASIC_H_ */
