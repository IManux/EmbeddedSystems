/*
 * stm32f4xx_usart_basic.h
 *
 *  Created on: Apr 1, 2022
 *      Author: ingma
 */

#ifndef INC_STM32F4XX_USART_BASIC_H_
#define INC_STM32F4XX_USART_BASIC_H_

struct usartx_t;


uint8_t usartx_init(struct usartx_t *usart, uint32_t bauds);
uint32_t usartx_write(struct usartx_t *usart, uint8_t *buf, uint32_t sz);
uint32_t usartx_print(struct usartx_t *usart, char *buf);
uint32_t usartx_print_char(struct usartx_t *usart, char ch);
uint32_t usartx_available_bytes(struct usartx_t *usart);
uint8_t usartx_get_byte(struct usartx_t *usart);
char usartx_get_char(struct usartx_t *usart);
uint32_t usartx_get_bytes(struct usartx_t *usart, uint8_t* buf, uint32_t len, uint32_t n);

extern struct usartx_t _usart1_drv;
#define usart1 (&_usart1_drv)

extern struct usartx_t _usart2_drv;
#define usart2 (&_usart2_drv)

extern struct usartx_t _usart3_drv;
#define usart3 (&_usart3_drv)

extern struct usartx_t _uart4_drv;
#define uart4 (&_uart4_drv)

extern struct usartx_t _uart5_drv;
#define uart5 (&_uart5_drv)

extern struct usartx_t _usart6_drv;
#define usart6 (&_usart6_drv)

#endif /* INC_STM32F4XX_USART_BASIC_H_ */
