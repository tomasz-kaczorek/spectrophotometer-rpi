#include "gpio.h"

#include <fcntl.h>	//open
#include <sys/mman.h>	//mmap
#include <sys/stat.h>	//open
#include <sys/types.h>	//open
#include <unistd.h>	//close

uint32_t const SPI_MOSI = 10;
uint32_t const SPI_MISO =  9;
uint32_t const SPI_SCLK = 11;
uint32_t const SPI_CS0  =  8;
uint32_t const SPI_CS1  =  7;

uint32_t const ADS_RST  = 18;
uint32_t const ADS_CS0  = 22;
uint32_t const ADS_CS1  = 23;
uint32_t const ADS_DRDY = 17;
uint32_t const ADS_PDWN = 27;
uint32_t const ADS_SYNC = 27;

uint32_t const OPT_A = 3;
uint32_t const OPT_B = 2;

uint32_t const AMP_A0 = 20;
uint32_t const AMP_A1 = 21;

uint32_t const MOT_M0     = 25;
uint32_t const MOT_M0_NE  = 24;
uint32_t const MOT_M1     = 5;
uint32_t const MOT_NSLEEP = 6;
uint32_t const MOT_STEP   = 12;
uint32_t const MOT_DIR    = 13;

uint32_t const LOW  = 0;
uint32_t const HIGH = 1;

uint32_t const IN   = 0x00000000;
uint32_t const OUT  = 0x00000001;
uint32_t const ALT0 = 0x00000004;
uint32_t const ALT1 = 0x00000005;
uint32_t const ALT2 = 0x00000006;
uint32_t const ALT3 = 0x00000007;
uint32_t const ALT4 = 0x00000003;
uint32_t const ALT5 = 0x00000002;

static uint32_t * volatile gpio_registers = NULL;

int gpio_init()
{
	//do nothing if already initialized
	if(gpio_registers) return 0;

	int fd = open("/dev/mem", O_RDWR | O_SYNC);
	if(fd == -1) return -1;

	//GPIO registers start at physical address 0x3F200000
	//GPIO has 41 32-bits registers
	void * map = mmap(NULL, 41 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x3F200000);

	close(fd);

	if(map == MAP_FAILED) return -1;

	gpio_registers = map;
	return 0;
}

int gpio_deinit()
{
	munmap(gpio_registers, 41 * 4);
	gpio_registers = NULL;
	return 0;
}

uint32_t gpio_get_fsel(uint32_t gpio)
{
	//function select registers begins at sRegisters + 0
	//each register controls functions of 10 GPIOs
	uint32_t reg = gpio_registers[gpio / 10];
	//extract 3  bits corresponding to the function of a specified GPIO
	return  0x00000007 & reg >> gpio % 10 * 3;
}

void gpio_set_fsel(uint32_t gpio, uint32_t function)
{
	//function select registers begins at sRegisters + 0
	//each register controls functions of 10 GPIOs
	uint32_t reg = gpio_registers[gpio / 10];
	//clear 3  bits corresponding to the function of a specified GPIO
	reg &= ~(0x00000007 << gpio % 10 * 3);
	//change the same 3 bits if a clear wasn't enough
	if(function) reg |= function << gpio % 10 * 3;
	gpio_registers[gpio / 10] = reg;
}

uint32_t gpio_get_level(uint32_t gpio)
{
	//level registers begin at sRegisters + 13
	//each register controls levels of 32 GPIOs
	if(gpio_registers[13 + gpio / 32] & 1 << gpio % 32) return HIGH;
	return LOW;
}

void gpio_set_level(uint32_t gpio, uint32_t level)
{
	//set registers begin at sRegisters + 7
	//clear registers begin at sRegisters + 10
	//each register controls levels of 32 GPIOs
	gpio_registers[(level == HIGH ? 7 : 10) + gpio / 32] = 1 << gpio % 32;
}
