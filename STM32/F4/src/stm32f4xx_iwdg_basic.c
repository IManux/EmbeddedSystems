/*
 * stm32f4xx_iwdg_basic.c
 *
 *
 */
#include "stm32f4xx.h"
#include "stm32f4xx_iwdg_basic.h"

/*
 * Private Variables
 * */
uint32_t iwdg_state = 0; // not init

/*
 * IWDG	FUNCTIONS
 */
uint32_t iwdg_init(uint32_t ms)
{
	uint32_t pr_reg;
	uint32_t pr_t;
	uint32_t rl_t;
	uint32_t ret = 1;

	if (iwdg_state) {
		return ret;
	}
	/*
	 * https://controllerstech.com/iwdg-and-wwdg-in-stm32/
	 * RL = 0 - 4095 [12 bits]
	 * PR = 0 - 6 [3 bits] = 4, 8, 16, 32, 64, 128, 256
	 * Formule:
	 * RL = ((Time[ms] x 32000 ) / (4 x 2^PR x 1000)) - 1
	 * Example: for Time = 20 ms
	 * PR = 0
	 * RL = (20 x 32000) / (4 x 2^0 x 1000)
	 * RL = 159
	 *
	 * Time[ms] = (4 x 2^PR x 1000)(RL + 1) / 32000
	 * RL = 4095
	 * if PR = 0
	 * Time[ms] = (4 x 2^0 x 1000)(4095 + 1) / 32000
	 * Time[ms] = 512
	 * if PR = 1
	 * Time[ms] = 1024
	 * if PR = 2
	 * Time[ms] = 2048
	 * if PR = 3
	 * Time[ms] = 4096
	 * if PR = 4
	 * Time[ms] = 8192
	 * if PR = 5
	 * Time[ms] = 16384
	 * if PR = 6
	 * Time[ms] = 32768
	 *
	 */
	if (ms <= 512) {
		pr_reg = LL_IWDG_PRESCALER_4;		// clock prescaler 4
		pr_t = 1;
	}
	else if (ms <= 1024) {
		pr_reg = LL_IWDG_PRESCALER_8;		// clock prescaler 8
		pr_t = 2;
	}
	else if (ms <= 2048) {
		pr_reg = LL_IWDG_PRESCALER_16;		// clock prescaler 16
		pr_t = 4;
	}
	else if (ms <= 4096) {
		pr_reg = LL_IWDG_PRESCALER_32;		// clock prescaler 32
		pr_t = 8;
	}
	else if (ms <= 8192) {
		pr_reg = LL_IWDG_PRESCALER_64;		// clock prescaler 64
		pr_t = 16;
	}
	else if (ms <= 16384) {
		pr_reg = LL_IWDG_PRESCALER_128;		// clock prescaler 128
		pr_t = 32;
	}
	else if (ms <= 32768) {
		pr_reg = LL_IWDG_PRESCALER_256;		// clock prescaler 256
		pr_t = 64;
	} else {
		ret = 0;
	}

	if (ret) {	// calculate RL
		uint32_t dp;
		uint32_t np;
		np = ms * 32000;
		dp = 4 * pr_t * 1000;
		rl_t = np / dp;
		rl_t = rl_t - 1;

		LL_IWDG_Enable(IWDG);
		LL_IWDG_EnableWriteAccess(IWDG);
		LL_IWDG_SetPrescaler(IWDG, pr_reg);
		LL_IWDG_SetReloadCounter(IWDG, rl_t);
		LL_IWDG_DisableWriteAccess(IWDG);

		while (LL_IWDG_IsReady(IWDG) != 1) {

		}
		iwdg_state = 1;
	}

	return ret;
}

void iwdg_reset()
{
	if (iwdg_state) {
		LL_IWDG_ReloadCounter(IWDG);
	}
}
