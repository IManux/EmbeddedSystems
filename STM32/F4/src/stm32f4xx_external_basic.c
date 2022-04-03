/*
 * stm32f4xx_external_basic.c
 *
 *
 */
#include "stm32f4xx.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_external_basic.h"


typedef void (*__ext_line_callback_t)(void);
static void __ext_line_cb_default(void);

__ext_line_callback_t __ext_line0_cb = &__ext_line_cb_default;
__ext_line_callback_t __ext_line1_cb = &__ext_line_cb_default;
__ext_line_callback_t __ext_line2_cb = &__ext_line_cb_default;
__ext_line_callback_t __ext_line3_cb = &__ext_line_cb_default;
__ext_line_callback_t __ext_line4_cb = &__ext_line_cb_default;
__ext_line_callback_t __ext_line5_cb = &__ext_line_cb_default;
__ext_line_callback_t __ext_line6_cb = &__ext_line_cb_default;
__ext_line_callback_t __ext_line7_cb = &__ext_line_cb_default;
__ext_line_callback_t __ext_line8_cb = &__ext_line_cb_default;
__ext_line_callback_t __ext_line9_cb = &__ext_line_cb_default;
__ext_line_callback_t __ext_line10_cb = &__ext_line_cb_default;
__ext_line_callback_t __ext_line11_cb = &__ext_line_cb_default;
__ext_line_callback_t __ext_line12_cb = &__ext_line_cb_default;
__ext_line_callback_t __ext_line13_cb = &__ext_line_cb_default;
__ext_line_callback_t __ext_line14_cb = &__ext_line_cb_default;
__ext_line_callback_t __ext_line15_cb = &__ext_line_cb_default;


/**************************
	Private Functions
***************************/
static void __ext_line_cb_default(void)
{

}

static IRQn_Type __ext_get_irq(uint32_t ext)
{
	IRQn_Type irq;
	if (ext == 0) {
		irq = EXTI0_IRQn;
	}
	else if (ext == 1) {
		irq = EXTI1_IRQn;
	}
	else if (ext == 2) {
		irq = EXTI2_IRQn;
	}
	else if (ext == 3) {
		irq = EXTI3_IRQn;
	}
	else if (ext == 4) {
		irq = EXTI4_IRQn;
	}
	else if (ext > 4 && ext < 10) {
		irq = EXTI9_5_IRQn;
	}
	else if (ext > 9 && ext < 16) {
		irq = EXTI15_10_IRQn;
	}
	else {
		irq = EXTI0_IRQn;
	}

	return irq;
}

/**************************
	Public Functions
***************************/
/*
 *
 * @param
 * - ext: 0 - 15.
 * - port: 0 - 4 (A - E).
 * - spull: 0 (PULL NO), 1 (PULL UP), 2 (PULL DOWN).
 * - trigger: 0 (none), 1 (rising), 2 (falling), 3 (rising & falling).
 *
 * */
uint32_t external_set_line(uint32_t ext, uint32_t port, uint8_t spull, uint8_t trigger)
{
	uint32_t pin, ahb1;
	LL_EXTI_InitTypeDef EXTI_InitStruct = {0};
	GPIO_TypeDef *gpio;
	uint32_t lookup_ext_line[16] = {
								0x000F0000UL,	//	EXT0
								0x00F00000UL,	//	EXT1
								0x0F000000UL,	//	EXT2
								0xF0000000UL,	//	EXT3
								0x000F0001UL,	//	EXT4
								0x00F00001UL,	//	EXT5
								0x0F000001UL,	//	EXT6
								0xF0000001UL,	//	EXT7
								0x000F0002UL,	//	EXT8
								0x00F00002UL,	//	EXT9
								0x0F000002UL,	//	EXT10
								0xF0000002UL,	//	EXT11
								0x000F0003UL,	//	EXT12
								0x00F00003UL,	//	EXT13
								0x0F000003UL,	//	EXT14
								0xF0000003UL	//	EXT15
	};

	if (ext > 15 || port > 4 || trigger > 3 || spull > 2) {
		return 0;
	}

	//get GPIO
	if (port == 0) {
		gpio = GPIOA;
	}
	else if (port == 1) {
		gpio = GPIOB;
	}
	else if (port == 2) {
		gpio = GPIOC;
	}
	else if (port == 3) {
		gpio = GPIOD;
	}
	else if (port == 4) {
		gpio = GPIOE;
	}

	pin = 0x1UL << ext;
	ahb1 = 0x1UL << port;
	/* GPIO Port Clock Enable */
	LL_AHB1_GRP1_EnableClock(ahb1);
	/**/
	LL_SYSCFG_SetEXTISource(port, lookup_ext_line[ext]);

	/**/
	EXTI_InitStruct.Line_0_31 = pin; //	LL_EXTI_LINE_13;
	EXTI_InitStruct.LineCommand = 1; // ENABLE
	EXTI_InitStruct.Mode = 0x00U; //	LL_EXTI_MODE_IT;
	EXTI_InitStruct.Trigger = trigger;	//LL_EXTI_TRIGGER_FALLING;
	LL_EXTI_Init(&EXTI_InitStruct);

	/**/
	LL_GPIO_SetPinMode(gpio, pin, LL_GPIO_MODE_INPUT);
	LL_GPIO_SetPinPull(gpio, pin, spull);
	LL_GPIO_SetPinSpeed(gpio, pin, LL_GPIO_SPEED_FREQ_HIGH);

	return 0;
}

void external_enable_interrupt(uint32_t ext)
{
	if (ext < 16) {
		IRQn_Type irq;
		irq = __ext_get_irq(ext);
		NVIC_SetPriority(irq, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
		NVIC_EnableIRQ(irq);
	}
}

void external_disable_interrupt(uint32_t ext)
{
	if (ext < 16) {
		IRQn_Type irq;
		irq = __ext_get_irq(ext);
		NVIC_DisableIRQ(irq);
	}
}

void external_set_callback(uint32_t ext, void(*extcb)(void))
{

	if (ext == 0) {
		__ext_line0_cb = extcb;
	}
	else if (ext == 1) {
		__ext_line1_cb = extcb;
	}
	else if (ext == 2) {
		__ext_line2_cb = extcb;
	}
	else if (ext == 3) {
		__ext_line3_cb = extcb;
	}
	else if (ext == 4) {
		__ext_line4_cb = extcb;
	}
	else if (ext == 5) {
		__ext_line5_cb = extcb;
	}
	else if (ext == 6) {
		__ext_line6_cb = extcb;
	}
	else if (ext == 7) {
		__ext_line7_cb = extcb;
	}
	else if (ext == 8) {
		__ext_line8_cb = extcb;
	}
	else if (ext == 9) {
		__ext_line9_cb = extcb;
	}
	else if (ext == 10) {
		__ext_line10_cb = extcb;
	}
	else if (ext == 11) {
		__ext_line11_cb = extcb;
	}
	else if (ext == 12) {
		__ext_line12_cb = extcb;
	}
	else if (ext == 13) {
		__ext_line13_cb = extcb;
	}
	else if (ext == 14) {
		__ext_line14_cb = extcb;
	}
	else if (ext == 15) {
		__ext_line15_cb = extcb;
	}
}


/**************************************************************
			IRQ Handlers
***************************************************************/
void EXTI0_IRQHandler(void)   // <----- The ISR Function We're Looking For!
{
	if (LL_EXTI_IsActiveFlag_0_31(0x01UL)) {
		LL_EXTI_ClearFlag_0_31(0x01UL);
		__ext_line0_cb();
	}
}

void EXTI1_IRQHandler(void)   // <----- The ISR Function We're Looking For!
{
	if (LL_EXTI_IsActiveFlag_0_31(0x02UL)) {
		LL_EXTI_ClearFlag_0_31(0x02UL);
		__ext_line1_cb();
	}
}

void EXTI2_IRQHandler(void)   // <----- The ISR Function We're Looking For!
{
	if (LL_EXTI_IsActiveFlag_0_31(0x04UL)) {
		LL_EXTI_ClearFlag_0_31(0x04UL);
		__ext_line2_cb();
	}
}

void EXTI3_IRQHandler(void)   // <----- The ISR Function We're Looking For!
{
	if (LL_EXTI_IsActiveFlag_0_31(0x08UL)) {
		LL_EXTI_ClearFlag_0_31(0x08UL);
		__ext_line3_cb();
	}
}

void EXTI4_IRQHandler(void)   // <----- The ISR Function We're Looking For!
{
	if (LL_EXTI_IsActiveFlag_0_31(0x10UL)) {
		LL_EXTI_ClearFlag_0_31(0x10UL);
		__ext_line4_cb();
	}
}

void EXTI9_5_IRQHandler(void)   // <----- The ISR Function We're Looking For!
{
	if (LL_EXTI_IsActiveFlag_0_31(0x20UL)) {	//EXT5
		LL_EXTI_ClearFlag_0_31(0x20UL);
		__ext_line5_cb();
	}
	else if (LL_EXTI_IsActiveFlag_0_31(0x40UL)) {	//EXT6
		LL_EXTI_ClearFlag_0_31(0x40UL);
		__ext_line6_cb();
	}
	else if (LL_EXTI_IsActiveFlag_0_31(0x80UL)) {	//EXT7
		LL_EXTI_ClearFlag_0_31(0x80UL);
		__ext_line7_cb();
	}
	else if (LL_EXTI_IsActiveFlag_0_31(0x100UL)) {	//EXT8
		LL_EXTI_ClearFlag_0_31(0x100UL);
		__ext_line8_cb();
	}
	else if (LL_EXTI_IsActiveFlag_0_31(0x200UL)) {	//EXT9
		LL_EXTI_ClearFlag_0_31(0x200UL);
		__ext_line9_cb();
	}

}

///*
void EXTI15_10_IRQHandler(void)   // <----- The ISR Function We're Looking For!
{
	if (LL_EXTI_IsActiveFlag_0_31(0x400UL)) {	//EXT10
		LL_EXTI_ClearFlag_0_31(0x400UL);
		__ext_line10_cb();
	}
	else if (LL_EXTI_IsActiveFlag_0_31(0x800UL)) {	//EXT11
		LL_EXTI_ClearFlag_0_31(0x800UL);
		__ext_line11_cb();
	}
	else if (LL_EXTI_IsActiveFlag_0_31(0x1000UL)) {	//EXT12
		LL_EXTI_ClearFlag_0_31(0x1000UL);
		__ext_line12_cb();
	}
	else if (LL_EXTI_IsActiveFlag_0_31(0x2000UL)) {	//EXT13
		LL_EXTI_ClearFlag_0_31(0x2000UL);
		__ext_line13_cb();
	}
	else if (LL_EXTI_IsActiveFlag_0_31(0x4000UL)) {	//EXT14
		LL_EXTI_ClearFlag_0_31(0x4000UL);
		__ext_line14_cb();
	}
	else if (LL_EXTI_IsActiveFlag_0_31(0x8000UL)) {	//EXT15
		LL_EXTI_ClearFlag_0_31(0x8000UL);
		__ext_line15_cb();
	}
}
//*/
