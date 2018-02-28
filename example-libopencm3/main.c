#include <stdlib.h>
#include <string.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/syscfg.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

int main(void)
{
	rcc_clock_setup_hse_3v3(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);
	rcc_periph_clock_enable(RCC_GPIOB);
	gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO3);
	while(1) {
		/* Toggle PB3 (Connected to LED) */
		gpio_toggle(GPIOB, GPIO3);
		/* busy wait pause :s */
		for(unsigned int i = 0; i < 4000000; i += 1) __asm__("nop");
	}

	return 0;
}
