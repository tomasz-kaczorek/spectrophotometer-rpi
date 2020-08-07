#ifndef ADS_H
#define ADS_H

#include <stdint.h>

extern uint8_t const AIN0;
extern uint8_t const AIN1;
extern uint8_t const AIN2;
extern uint8_t const AIN3;
extern uint8_t const AIN4;
extern uint8_t const AIN5;
extern uint8_t const AIN6;
extern uint8_t const AIN7;
extern uint8_t const AINCOM;

extern uint8_t const SPS_30000;
extern uint8_t const SPS_15000;
extern uint8_t const SPS_7500;
extern uint8_t const SPS_3750;
extern uint8_t const SPS_2000;
extern uint8_t const SPS_1000;
extern uint8_t const SPS_500;
extern uint8_t const SPS_100;
extern uint8_t const SPS_60;
extern uint8_t const SPS_50;
extern uint8_t const SPS_30;
extern uint8_t const SPS_25;
extern uint8_t const SPS_15;
extern uint8_t const SPS_10;
extern uint8_t const SPS_5;
extern uint8_t const SPS_2_5;

int ads_init();
int ads_deinit();

void ads_reset();
uint8_t ads_get_status();
uint8_t ads_get_channel();
void ads_set_channel(uint8_t channel);
uint8_t ads_get_data_rate();
void ads_set_data_rate(uint8_t drate);
int32_t ads_read_data();

#endif // ADS_H
