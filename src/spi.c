#include "spi.h"

#include "gpio.h"

#include <fcntl.h>      //open
#include <sys/mman.h>   //mmap
#include <sys/stat.h>   //open
#include <sys/types.h>  //open
#include <unistd.h>     //close

uint32_t const MODE0 = 0x00000000;
uint32_t const MODE1 = 0x00000004;
uint32_t const MODE2 = 0x00000008;
uint32_t const MODE3 = 0x0000000C;

static int const CS   = 0;
static int const FIFO = 1;
static int const CLK  = 2;
static int const DLEN = 3;
static int const LTOH = 4;
static int const DC   = 5;

static uint32_t const LEN_LONG = 0x02000000;
static uint32_t const DMA_LEN  = 0x01000000;
static uint32_t const CSPOL2   = 0x00800000;
static uint32_t const CSPOL1   = 0x00400000;
static uint32_t const CSPOL0   = 0x00200000;
static uint32_t const RXF      = 0x00100000;
static uint32_t const RXR      = 0x00080000;
static uint32_t const TXD      = 0x00040000;
static uint32_t const RXD      = 0x00020000;
static uint32_t const DONE     = 0x00010000;
static uint32_t const TE_EN    = 0x00008000;
static uint32_t const LMONO    = 0x00004000;
static uint32_t const LEN      = 0x00002000;
static uint32_t const REN      = 0x00001000;
static uint32_t const ADCS     = 0x00000800;
static uint32_t const INTR     = 0x00000400;
static uint32_t const INTD     = 0x00000200;
static uint32_t const DMAEN    = 0x00000100;
static uint32_t const TA       = 0x00000080;
static uint32_t const CSPOL    = 0x00000040;
static uint32_t const CLEAR_RX = 0x00000020;
static uint32_t const CLEAR_TX = 0x00000010;
static uint32_t const CPOL     = 0x00000008;
static uint32_t const CPHA     = 0x00000004;
static uint32_t const CS2      = 0x00000002;
static uint32_t const CS1      = 0x00000001;
static uint32_t const CS0      = 0x00000000;

static int32_t * volatile spi_registers = NULL;

int spi_init()
{
	//initialization cannot be completed without access to GPIOs
	if(gpio_init()) return -1;
	
	//do nothing if already initialized
	if(spi_registers) return 0;

	int fd = open("/dev/mem", O_RDWR | O_SYNC);
	if(fd == -1) return -1;

	//SPI registers start at physical address 0x3F204000
	//SPI has 6 32-bits registers
	void * map = mmap(NULL, 6 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x3F204000);
	
	close(fd);
	
	if(map == MAP_FAILED) return -1;

	spi_registers = map;
	spi_registers[CS] = 0;
	gpio_set_fsel(SPI_MOSI, ALT0);
	gpio_set_fsel(SPI_MISO, ALT0);
	gpio_set_fsel(SPI_SCLK, ALT0);
	gpio_set_fsel(SPI_CS0, ALT0);
	gpio_set_fsel(SPI_CS1, ALT0);

	return 0;
}

int spi_deinit()
{
	if(gpio_init()) return -1;
	
	munmap(spi_registers, 6 * 4);
        spi_registers = NULL;
	gpio_set_fsel(SPI_MOSI, IN);
	gpio_set_fsel(SPI_MISO, IN);
	gpio_set_fsel(SPI_SCLK, IN);
	gpio_set_fsel(SPI_CS0, IN);
	gpio_set_fsel(SPI_CS1, IN);
	
	return 0;
}

void spi_set_mode(uint32_t mode)
{
	spi_registers[CS] = (spi_registers[CS] & 0xFFFFFFF0) | mode;
}

void spi_set_clock_divider(uint32_t divider)
{
	spi_registers[CLK] = divider;
}

void spi_transfer(uint8_t * txd, uint8_t * rxd)
{
	//clear FIFOs and set Transfer Active
	spi_registers[CS] |= CLEAR_TX | CLEAR_RX | TA;
	//wait until FIFO can accept data
	while(!(spi_registers[CS] & TXD));
	spi_registers[FIFO] = *txd;
	//wait until FIFO can provide data
	while(!(spi_registers[CS] & RXD));
	*rxd = spi_registers[FIFO];
	//wait until SPI finish
	while(!(spi_registers[CS] & DONE));
	//clear Transfer Active
	spi_registers[CS] &= ~TA;
}
