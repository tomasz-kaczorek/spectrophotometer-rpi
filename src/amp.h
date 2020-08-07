#ifndef AMP_H
#define AMP_H

#include <stdint.h>

extern uint32_t const GAIN_1;
extern uint32_t const GAIN_10;
extern uint32_t const GAIN_100;
extern uint32_t const GAIN_1000;

int amp_init();
int amp_deinit();

uint32_t amp_get_gain();
void amp_set_gain(uint32_t gain);

#endif //AMP_H
