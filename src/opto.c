#include "opto.h"

#include "gpio.h"
#include "spi.h"
#include "ads.h"

int opto_init()
{
	//initialization cannot be completed without access to GPIOs
        if(gpio_init()) return -1;
	//initialization cannot be completed without access to SPI
        if(spi_init()) return -1;
	//initialization cannot be completed without access to ADS
        if(ads_init()) return -1;

	gpio_set_fsel(OPT_A, OUT);
        gpio_set_fsel(OPT_B, OUT);

	gpio_set_level(OPT_A, LOW);
        gpio_set_level(OPT_B, LOW);

	return 0;
}

int opto_deinit()
{
	if(gpio_init()) return -1;

        gpio_set_fsel(OPT_A, IN);
        gpio_set_fsel(OPT_B, IN);

	return 0;
}

uint32_t opto_get_a()
{
	return gpio_get_level(OPT_A);
}

void opto_set_a(uint32_t level)
{
	gpio_set_level(OPT_A, level);
}

int32_t opto_read_a()
{
	ads_set_channel(AIN1 << 4 | AIN0);
	return ads_read_data();
}

uint32_t opto_get_b()
{
	return gpio_get_level(OPT_B);
}

void opto_set_b(uint32_t level)
{
	gpio_set_level(OPT_B, level);
}

int32_t opto_read_b()
{
	ads_set_channel(AIN2 << 4 | AIN0);
	return ads_read_data();
}
