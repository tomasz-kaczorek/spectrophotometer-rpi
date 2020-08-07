#ifndef ARGS_H
#define ARGS_H

#include <stdint.h>

void process_pin(uint32_t pin, char * value);

void process_data_rate(char * value);
void process_channel(char * value);
void process_data(char * value);

void process_a(char * value);
void process_b(char * value);

void process_active(char * value);
void process_direction(char * value);
void process_step_divider(char * value);
void process_steps(char * value);

void process_gain(char * value);

void process_offset(char * value);

void process_count(char * value);

#endif //ARGS_H
