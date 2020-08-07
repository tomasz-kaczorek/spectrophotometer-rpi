#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>

extern uint32_t const RED_TO_BLUE;
extern uint32_t const BLUE_TO_RED;

extern uint32_t const DIV_1;
extern uint32_t const DIV_2;
extern uint32_t const DIV_4;
extern uint32_t const DIV_8;
extern uint32_t const DIV_16;
extern uint32_t const DIV_32;

int motor_init();
int motor_deinit();

uint32_t motor_get_step_divider();
void motor_set_step_divider(uint32_t divider);
uint32_t motor_get_direction();
void motor_set_direction(uint32_t direction);
uint32_t motor_get_active();
void motor_set_active(uint32_t active);
void motor_step();

#endif //MOTOR_H
