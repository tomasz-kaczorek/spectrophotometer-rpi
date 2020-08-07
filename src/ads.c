#include "ads.h"

#include "gpio.h"
#include "spi.h"

#include <unistd.h>	//usleep

uint8_t const AIN0   = 0x00;
uint8_t const AIN1   = 0x01;
uint8_t const AIN2   = 0x02;
uint8_t const AIN3   = 0x03;
uint8_t const AIN4   = 0x04;
uint8_t const AIN5   = 0x05;
uint8_t const AIN6   = 0x06;
uint8_t const AIN7   = 0x07;
uint8_t const AINCOM = 0x08;

uint8_t const SPS_30000 = 0xF0;
uint8_t const SPS_15000 = 0xE0;
uint8_t const SPS_7500  = 0xD0;
uint8_t const SPS_3750  = 0xC0;
uint8_t const SPS_2000  = 0xB0;
uint8_t const SPS_1000  = 0xA1;
uint8_t const SPS_500   = 0xA2;
uint8_t const SPS_100   = 0x82;
uint8_t const SPS_60    = 0x72;
uint8_t const SPS_50    = 0x63;
uint8_t const SPS_30    = 0x53;
uint8_t const SPS_25    = 0x43;
uint8_t const SPS_15    = 0x33;
uint8_t const SPS_10    = 0x23;
uint8_t const SPS_5     = 0x13;
uint8_t const SPS_2_5   = 0x03;

static uint8_t const STATUS = 0x00;
static uint8_t const MUX    = 0x01;
static uint8_t const ADCON  = 0x02;
static uint8_t const DRATE  = 0x03;
static uint8_t const IO     = 0x04;
static uint8_t const OFC0   = 0x05;
static uint8_t const OFC1   = 0x06;
static uint8_t const OFC2   = 0x07;
static uint8_t const FSC0   = 0x08;
static uint8_t const FSC1   = 0x09;
static uint8_t const FSC2   = 0x0A;

static uint8_t const WAKEUP   = 0x00;
static uint8_t const RDATA    = 0x01;
static uint8_t const RDATAC   = 0x03;
static uint8_t const SDATAC   = 0x0F;
static uint8_t const RREG     = 0x10;
static uint8_t const WREG     = 0x50;
static uint8_t const SELFCAL  = 0xF0;
static uint8_t const SELFOCAL = 0xF1;
static uint8_t const SELFGCAL = 0xF2;
static uint8_t const SYSOCAL  = 0xF3;
static uint8_t const SYSGCAL  = 0xF4;
static uint8_t const SYNC     = 0xFC;
static uint8_t const STANDBY  = 0xFD;
static uint8_t const RESET    = 0xFE;

uint8_t ads_read_register(uint8_t reg)
{
	uint8_t txd, rxd;
	gpio_set_level(ADS_CS0, LOW);
	
	txd = RREG | reg;	
	spi_transfer(&txd, &rxd);

	txd = 0x00;
	spi_transfer(&txd, &rxd);

	//min 50 x T_CLKIN before colecting response
	usleep(7);
	spi_transfer(&txd, &rxd);
	
	gpio_set_level(ADS_CS0, HIGH);
	//min 4 x T_CLKIN before next command
	usleep(1);
	
	return rxd;
}

void ads_write_register(uint8_t reg, uint8_t data)
{
	uint8_t txd, rxd;
	gpio_set_level(ADS_CS0, LOW);
	
	txd = WREG | reg;	
	spi_transfer(&txd, &rxd);

	txd = 0x00;
	spi_transfer(&txd, &rxd);

	txd = data;
	spi_transfer(&txd, &rxd);
	//min 8 x T_CLKIN of low CS after last SPI clock edge	
	usleep(2);
	gpio_set_level(ADS_CS0, HIGH);
	//min 4 x T_CLKIN before next command
	usleep(1);
}

int ads_init()
{
	//initialization cannot be completed without access to GPIOs
	if(gpio_init()) return -1;
	//initialization cannot be completed without access to GPIOs
	if(spi_init()) return -1;

	gpio_set_fsel(ADS_RST, OUT);
	gpio_set_level(ADS_RST, HIGH);
	gpio_set_fsel(ADS_CS0, OUT);
	gpio_set_level(ADS_CS0, HIGH);
	gpio_set_fsel(ADS_PDWN, OUT);
	gpio_set_level(ADS_PDWN, HIGH);
	gpio_set_fsel(ADS_DRDY, IN);

	spi_set_mode(MODE1);
	//T_SCLK is 8 ns (125 MHz clock on RPi board)
	//T_CLKIN is 130.208 ns (7.68 MHz clock on ADS board)
	//T_DATA is 33.(3) us - 40 ms (2.5 Hz to 30 kHz data rate)
	//min 4 x T_CLKIN (~520.8 ns) max 10 x T_DATA (~333.3 us) for T_SCLK
	spi_set_clock_divider(128);

	return 0;
}

int ads_deinit()
{
	if(gpio_init()) return -1;
	gpio_set_fsel(ADS_RST, IN);
	gpio_set_fsel(ADS_CS0, IN);
	return 0;
}

void ads_reset()
{
	gpio_set_level(ADS_RST, 0);
	//min 4 x T_CLKIN for reset to take effect
	usleep(1);
	
	gpio_set_level(ADS_RST, 1);
	
	//ADS_DRDY == LOW when reset is completed
	while(gpio_get_level(ADS_DRDY) == HIGH);
}

uint8_t ads_get_status()
{
	return ads_read_register(STATUS);
}

uint8_t ads_get_channel()
{
	return ads_read_register(MUX);
}

void ads_set_channel(uint8_t channel)
{
	ads_write_register(MUX, channel);
}

uint8_t ads_get_data_rate()
{
	return ads_read_register(DRATE);
}

void ads_set_data_rate(uint8_t drate)
{
	ads_write_register(DRATE, drate);
}

int32_t ads_read_data()
{
	gpio_set_level(ADS_SYNC, LOW);
	//min 4 x T_CLKIN for sync to take effect
	usleep(1);

	gpio_set_level(ADS_SYNC, HIGH);

	//ADS_DRDY == LOW when data is ready
	while(gpio_get_level(ADS_DRDY));

	uint8_t txd, rxd;
	int32_t ret;
	gpio_set_level(ADS_CS0, LOW);
	
	txd = RDATA;	
	spi_transfer(&txd, &rxd);
	//min 50 x T_CLKIN before colecting response
	usleep(7);

	txd = 0x00;
	spi_transfer(&txd, &rxd);
	//carry sign bit to most significant byte
	ret = rxd & 0x80 ? rxd & 0xFF00 : rxd;
	//min 4 x T_CLKIN before next command
	usleep(1);
	spi_transfer(&txd, &rxd);
	ret = ret << 8 | rxd;
	//min 4 x T_CLKIN before next command
	usleep(1);
	spi_transfer(&txd, &rxd);
	ret = ret << 8 | rxd;
	//min 4 x T_CLKIN before next command
	usleep(1);
	
	gpio_set_level(ADS_CS0, HIGH);
	return ret;
}
