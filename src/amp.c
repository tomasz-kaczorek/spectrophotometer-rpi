#include "amp.h"

#include "gpio.h"

uint32_t const GAIN_1    = 0x00000000;
uint32_t const GAIN_10   = 0x00000001;
uint32_t const GAIN_100  = 0x00000002;
uint32_t const GAIN_1000 = 0x00000003;

int amp_init()
{
	//initialization cannot be completed without access to GPIOs
        if(gpio_init()) return -1;

	gpio_set_fsel(AMP_A0, OUT);
        gpio_set_fsel(AMP_A1, OUT);

	gpio_set_level(AMP_A0, LOW);
        gpio_set_level(AMP_A1, LOW);

	return 0;
}

int amp_deinit()
{
	if(gpio_init()) return -1;

        gpio_set_fsel(AMP_A0, IN);
        gpio_set_fsel(AMP_A1, IN);

	return 0;
}

uint32_t amp_get_gain()
{
	uint32_t A0 = gpio_get_level(AMP_A0);
	uint32_t A1 = gpio_get_level(AMP_A1);

	return A1 << 1 | A0;
}

void amp_set_gain(uint32_t gain)
{
	gpio_set_level(AMP_A0, gain & 0x00000001 ? HIGH : LOW);
	gpio_set_level(AMP_A1, gain & 0x00000002 ? HIGH : LOW);
}
