#ifndef OPTO_H
#define OPTO_H

#include <stdint.h>

int opto_init();
int opto_deinit();

uint32_t opto_get_a();
void opto_set_a(uint32_t level);
int32_t opto_read_a();
uint32_t opto_get_b();
void opto_set_b(uint32_t level);
int32_t opto_read_b();

#endif //OPTO_H
