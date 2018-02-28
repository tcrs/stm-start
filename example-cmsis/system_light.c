#include <stm32f407xx.h>

void _exit(int);
int main();

void SystemInit()
{
	/* Enable Power Control clock */
	RCC->APB1ENR |= RCC_APB1LPENR_PWRLPEN;
	/* Regulator voltage scaling output selection: Scale 2 */
	//PWR->CR |= PWR_CR_VOS_1;
 
	/* Wait until HSI ready */
	while ((RCC->CR & RCC_CR_HSIRDY) == 0);
 
	/* Store calibration value */
	PWR->CR |= (uint32_t)(16 << 3);
 
	/* Disable main PLL */
	RCC->CR &= ~(RCC_CR_PLLON);
	/* Wait until PLL ready (disabled) */
	while ((RCC->CR & RCC_CR_PLLRDY) != 0);
 
	/*
	 * Configure Main PLL
	 * HSI as clock input
	 * fvco = 336MHz
	 * fpllout = 84MHz
	 * fusb = 48MHz
	 * PLLM = 16
	 * PLLN = 336
	 * PLLP = 4
	 * PLLQ = 7
	 */
	RCC->PLLCFGR = (uint32_t)((uint32_t)0x20000000 | (uint32_t)(16 << 0) | (uint32_t)(336 << 6) | 
					RCC_PLLCFGR_PLLP_0 | (uint32_t)(7 << 24));
 
	/* PLL On */
	RCC->CR |= RCC_CR_PLLON;
	/* Wait until PLL is locked */
	while ((RCC->CR & RCC_CR_PLLRDY) == 0);
 
	/* 
	 * FLASH configuration block
	 * enable instruction cache
	 * enable prefetch
	 * set latency to 2WS (3 CPU cycles)
	 */
	FLASH->ACR |= FLASH_ACR_ICEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_2WS;
 
	/* Check flash latency */
	if ((FLASH->ACR & FLASH_ACR_LATENCY) != FLASH_ACR_LATENCY_2WS) {
		_exit(1);
	}
 
	/* Set clock source to PLL */
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	/* Check clock source */
	while ((RCC->CFGR & RCC_CFGR_SWS_PLL) != RCC_CFGR_SWS_PLL);
 
	/* Set HCLK (AHB1) prescaler (DIV1) */
	RCC->CFGR &= ~(RCC_CFGR_HPRE);
 
	/* Set APB1 Low speed prescaler (APB1) DIV2 */
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
 
	/* SET APB2 High speed srescaler (APB2) DIV1 */
	RCC->CFGR &= ~(RCC_CFGR_PPRE2);
}

void _exit(int code) {
	while(1);
}
