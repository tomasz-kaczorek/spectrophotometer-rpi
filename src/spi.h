#ifndef SPI_H
#define SPI_H

#include <stdint.h>

//transmission modes
extern uint32_t const MODE0;
extern uint32_t const MODE1;
extern uint32_t const MODE2;
extern uint32_t const MODE3;

int spi_init();
int spi_deinit();

void spi_set_mode(uint32_t mode);
void spi_set_clock_divider(uint32_t divider);
void spi_transfer(uint8_t * txd, uint8_t * rxd);

#endif //SPI_H
