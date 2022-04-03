/*
 * stm32f4xx_usart_basic.c
 *
 *  Created on: Apr 1, 2022
 *      Author: ingma
 *
 *
 *
 *
 *    HW		TX		RX		AF
 *	USART1		PA9		PA10
 *	USART2		PA2		PA3		7
 *
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f4xx.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_utils.h"

#include "stm32f4xx_usart_basic.h"


#define USART1_QUEUE_SIZE	128
#define USART2_QUEUE_SIZE	128
#define USART3_QUEUE_SIZE	128
#define UART4_QUEUE_SIZE	128
#define UART5_QUEUE_SIZE	128
#define USART6_QUEUE_SIZE	128

//Queue
struct usart_queue_t {
  uint32_t qhead;
  uint32_t qtail;
  uint32_t qmax;
  uint8_t *qbuff;
};

struct usartx_t {
	USART_TypeDef *hw;
	uint32_t state;
	struct usart_queue_t queue_rx;
};

/*	usart handle definition */
struct usartx_t _usart1_drv = { USART1,
						.state = 0,
						.queue_rx = { 0, 0, 0, NULL }
};

struct usartx_t _usart2_drv = { USART2,
						.state = 0,
						.queue_rx = { 0, 0, 0, NULL }
};

struct usartx_t _usart3_drv = { USART3,
						.state = 0,
						.queue_rx = { 0, 0, 0, NULL }
};

struct usartx_t _uart4_drv = { UART4,
						.state = 0,
						.queue_rx = { 0, 0, 0, NULL }
};

struct usartx_t _uart5_drv = { UART5,
						.state = 0,
						.queue_rx = { 0, 0, 0, NULL }
};

struct usartx_t _usart6_drv = { USART6,
						.state = 0,
						.queue_rx = { 0, 0, 0, NULL }
};

/**************************
	Private Functions
***************************/
static void __usartx_queue_add(struct usart_queue_t *q, uint8_t b)
{
	uint32_t idx;

	//Ring Buffer Circle
	idx = (q->qhead + 1) % q->qmax;

	if (idx == q->qtail) {	//Queue Full?
		q->qtail = (q->qtail + 1) % q->qmax;	//Yes, overwrite buffer like rb circle
	}
	q->qbuff[q->qhead] = b;
	q->qhead = idx;
}

/**************************
	Public Functions
***************************/
uint8_t usartx_init(struct usartx_t *usart, uint32_t bauds)
{
	uint32_t tx_pin, rx_pin;
	uint32_t queue_sz;
	uint32_t gpiox_ahb1, gpio_afx;
	uint32_t vapbx, wapbx;

	IRQn_Type irq;
	GPIO_TypeDef *gpiox_port;
	LL_USART_InitTypeDef USART_InitStruct = {0};
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};


	if (usart->state) {	// was init before?
		return 1;
	}

	if (usart->hw == USART1) {	// Tested
		tx_pin = LL_GPIO_PIN_6;
		rx_pin = LL_GPIO_PIN_7;
		gpiox_port = GPIOB;
		irq = USART1_IRQn;
		queue_sz = USART1_QUEUE_SIZE;
		gpiox_ahb1 = LL_AHB1_GRP1_PERIPH_GPIOB;
		wapbx = 2;
		vapbx = LL_APB2_GRP1_PERIPH_USART1;
		gpio_afx = LL_GPIO_AF_7;
	}
	else if (usart->hw == USART2) {	// Tested
		tx_pin = LL_GPIO_PIN_2;
		rx_pin = LL_GPIO_PIN_3;
		gpiox_port = GPIOA;
		irq = USART2_IRQn;
		queue_sz = USART2_QUEUE_SIZE;
		gpiox_ahb1 = LL_AHB1_GRP1_PERIPH_GPIOA;
		wapbx = 1;
		vapbx = LL_APB1_GRP1_PERIPH_USART2;
		gpio_afx = LL_GPIO_AF_7;
	}
	else if (usart->hw == USART3) {
		tx_pin = LL_GPIO_PIN_10;
		rx_pin = LL_GPIO_PIN_11;
		gpiox_port = GPIOB;
		irq = USART3_IRQn;
		queue_sz = USART3_QUEUE_SIZE;
		gpiox_ahb1 = LL_AHB1_GRP1_PERIPH_GPIOB;
		wapbx = 1;
		vapbx = LL_APB1_GRP1_PERIPH_USART3;
		gpio_afx = LL_GPIO_AF_7;
	}
	else if (usart->hw == UART4) {
		tx_pin = LL_GPIO_PIN_10;
		rx_pin = LL_GPIO_PIN_11;
		gpiox_port = GPIOC;
		irq = UART4_IRQn;
		queue_sz = UART4_QUEUE_SIZE;
		gpiox_ahb1 = LL_AHB1_GRP1_PERIPH_GPIOC;
		wapbx = 1;
		vapbx = LL_APB1_GRP1_PERIPH_UART4;
		gpio_afx = LL_GPIO_AF_8;
	}
	else if (usart->hw == UART5) {
		tx_pin = LL_GPIO_PIN_12; // c12
		rx_pin = 0x0UL; // d2
		gpiox_port = GPIOC;
		irq = UART5_IRQn;
		queue_sz = UART4_QUEUE_SIZE;
		gpiox_ahb1 = LL_AHB1_GRP1_PERIPH_GPIOC;
		wapbx = 1;
		vapbx = LL_APB1_GRP1_PERIPH_UART5;
		gpio_afx = LL_GPIO_AF_8;
	}
	else if (usart->hw == USART6) {
		tx_pin = LL_GPIO_PIN_6;
		rx_pin = LL_GPIO_PIN_7;
		irq = USART6_IRQn;
		gpiox_port = GPIOC;
		queue_sz = USART6_QUEUE_SIZE;
		gpiox_ahb1 = LL_AHB1_GRP1_PERIPH_GPIOC;
		wapbx = 2;
		vapbx = LL_APB2_GRP1_PERIPH_USART6;
		gpio_afx = LL_GPIO_AF_8;
	}
	else {
		return 2;
	}

	/*	Allocate MEM  */
	usart->queue_rx.qbuff = (uint8_t*) malloc(queue_sz);

	if (usart->queue_rx.qbuff == NULL) {
		return 3;
	}

	usart->queue_rx.qmax = USART2_QUEUE_SIZE;

	/* init rcc */
	if (wapbx == 1) {
		LL_APB1_GRP1_EnableClock(vapbx);
	} else {
		LL_APB2_GRP1_EnableClock(vapbx);
	}

	LL_AHB1_GRP1_EnableClock(gpiox_ahb1);
	if (usart->hw == UART5) {
		LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);
	}

	/* init hw gpio */
	GPIO_InitStruct.Pin = tx_pin | rx_pin;	//LL_GPIO_PIN_2 | LL_GPIO_PIN_3;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Alternate = gpio_afx;
	LL_GPIO_Init(gpiox_port, &GPIO_InitStruct);

	if (usart->hw == UART5) { // setup RX - D2
		GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
		LL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	}

	/* init hw uart */
	USART_InitStruct.BaudRate = bauds;
	USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
	USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
	USART_InitStruct.Parity = LL_USART_PARITY_NONE;
	USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
	USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
	USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
	LL_USART_Init(usart->hw, &USART_InitStruct);
	LL_USART_ConfigAsyncMode(usart->hw);
	LL_USART_EnableIT_RXNE(usart->hw);


    /* NVIC USART Interrupt - Global */
    NVIC_SetPriority(irq, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(irq);

	/*	Enable USART  */
	LL_USART_Enable(usart->hw);

	usart->state = 1;

	return 0;
}

uint32_t usartx_write(struct usartx_t *usart, uint8_t *buf, uint32_t len)
{
	uint32_t i;
	for (i = 0; i < len; i++) {
		while (!(usart->hw->SR & USART_SR_TXE));
	    usart->hw->DR = buf[i];
	}
	while (!(usart->hw->SR & USART_SR_TC));

	return i;
}

uint32_t usartx_print(struct usartx_t *usart, char *buf)
{
	uint32_t len;
	uint8_t *p;

	len = strlen(buf);
	p = (uint8_t*) &buf[0];

	return usartx_write(usart, p, len);
}

uint32_t usartx_print_char(struct usartx_t *usart, char ch)
{
	uint8_t b;
	b = ch;
	return usartx_write(usart, &b, 1);
}

uint32_t usartx_available_bytes(struct usartx_t *usart)
{
	uint32_t n;

	n = (usart->queue_rx.qmax  + usart->queue_rx.qhead - usart->queue_rx.qtail) % usart->queue_rx.qmax;

	return n;
}

uint8_t usartx_get_byte(struct usartx_t *usart)
{
	uint8_t b;
	if (usart->queue_rx.qhead == usart->queue_rx.qtail) {	//queue empty?
			return 0;
	}
	b = usart->queue_rx.qbuff[usart->queue_rx.qtail];
	usart->queue_rx.qtail = (usart->queue_rx.qtail + 1) % usart->queue_rx.qmax;

	return b;
}

char usartx_get_char(struct usartx_t *usart)
{
	char ch;
	ch = usartx_get_byte(usart);
	return ch;
}

uint32_t usartx_get_bytes(struct usartx_t *usart, uint8_t* buf, uint32_t len, uint32_t n)
{
	uint32_t nb;
	uint32_t i;

	nb = usartx_available_bytes(usart);

	if (len == 0 || nb == 0) {
		n = 0;
	}

	if (n > nb) {
		n = nb;
	}

	if (n > len) {
		n = len;
	}

	for (i = 0; i < n; i++) {
		buf[i] = usartx_get_byte(usart);
	}

	return i;
}

uint32_t usartx_read_string_timeout(struct usartx_t *usart, char *buff, uint32_t len, uint32_t tout)
{
	uint32_t i;
	uint32_t n;
	char ch;

	i = 0;
	while (tout) {
		LL_mDelay(0);
		n = usartx_available_bytes(usart);
		while (n) {
			ch = usartx_get_byte(usart);
			buff[i] = ch;
			++i;
			if (i == len) {
				tout = 1;
				break;
			}
		}
		--tout;
	}

	buff[i] = 0;

	return i;
}

uint32_t usartx_read_string_until(struct usartx_t *usart, char *buff, uint32_t len, char terminator, uint32_t tout)
{
	uint32_t i;
	uint32_t n;
	char ch;

	i = 0;
	while (tout) {
		LL_mDelay(0);
		n = usartx_available_bytes(usart);
		while (n) {
			ch = usartx_get_byte(usart);
			buff[i] = ch;
			++i;
			if (i == len || ch == terminator) {
				tout = 1;
				break;
			}
		}
		--tout;
	}

	buff[i] = 0;

	return i;
}

/**************************************************************
			IRQ Handlers
***************************************************************/
static void __usartx_irq_handler(struct usartx_t *usart)
{
    uint32_t reg_sr;
    uint8_t b;

    reg_sr = usart->hw->SR;
    // receive buffer is not empty
    if(reg_sr & USART_SR_RXNE) {
        b = (uint8_t) usart->hw->DR;
        __usartx_queue_add(&usart->queue_rx, b);
    }
}

void USART1_IRQHandler(void)
{
    __usartx_irq_handler(usart1);
}

void USART2_IRQHandler(void)
{
    __usartx_irq_handler(usart2);
}

void USART3_IRQHandler(void)
{
    __usartx_irq_handler(usart3);
}

void UART4_IRQHandler(void)
{
    __usartx_irq_handler(uart4);
}

void UART5_IRQHandler(void)
{
    __usartx_irq_handler(uart5);
}

void USART6_IRQHandler(void)
{
    __usartx_irq_handler(usart6);
}
