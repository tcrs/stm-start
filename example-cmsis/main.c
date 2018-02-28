#include <stm32f4xx.h>

void main()
{
	/* Enable GPIOB clock */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	/* Set GPIOB3 as output */
	GPIOB->MODER = GPIO_MODER_MODER3_0;
	while(1) {
		/* Toggle PB3 (Connected to LED) */
		GPIOB->ODR ^= 0x8;
		/* busy wait pause :s */
		for(unsigned int i = 0; i < 4000000; i += 1) __asm__("nop");
	}
}
