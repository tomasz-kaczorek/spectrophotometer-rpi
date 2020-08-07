#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

//numbers of GPIOs for SPI
extern uint32_t const SPI_MOSI;
extern uint32_t const SPI_MISO;
extern uint32_t const SPI_SCLK;
extern uint32_t const SPI_CS0;
extern uint32_t const SPI_CS1;

//numbers of GPIOs for ADS
extern uint32_t const ADS_RST;
extern uint32_t const ADS_CS0;
extern uint32_t const ADS_CS1;
extern uint32_t const ADS_DRDY;
extern uint32_t const ADS_PDWN;
extern uint32_t const ADS_SYNC;

//numbers of GPIOs for optoswitches
extern uint32_t const OPT_A;
extern uint32_t const OPT_B;

//numbers of GPIOs for PGA
extern uint32_t const AMP_A0;
extern uint32_t const AMP_A1;

//numbers of GPIOs for motor
extern uint32_t const MOT_M0;
extern uint32_t const MOT_M0_NE;
extern uint32_t const MOT_M1;
extern uint32_t const MOT_NSLEEP;
extern uint32_t const MOT_STEP;
extern uint32_t const MOT_DIR;

//logic levels of GPIOs
extern uint32_t const LOW;
extern uint32_t const HIGH;

//functions of GPIOs
extern uint32_t const IN;
extern uint32_t const OUT;
extern uint32_t const ALT0;
extern uint32_t const ALT1;
extern uint32_t const ALT2;
extern uint32_t const ALT3;
extern uint32_t const ALT4;
extern uint32_t const ALT5;

int gpio_init();
int gpio_deinit();

uint32_t gpio_get_fsel(uint32_t gpio);
void gpio_set_fsel(uint32_t gpio, uint32_t function);
uint32_t gpio_get_level(uint32_t gpio);
void gpio_set_level(uint32_t gpio, uint32_t level);

#endif //GPIO_H
