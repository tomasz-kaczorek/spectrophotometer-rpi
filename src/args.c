#include "args.h"

#include "gpio.h"
#include "spi.h"
#include "ads.h"
#include "opto.h"
#include "motor.h"
#include "amp.h"
#include "tcp.h"

#include <stdio.h>
#include <string.h>

static uint32_t const gpio_to_pin[27] = {0,3,5,7,29,31,26,24,21,19,23,32,33,0,0,36,11,12,35,38,40,15,16,18,22,37,13};
static uint32_t const pin_to_gpio[40] = {0,0,2,0,3,0,4,14,0,15,17,18,27,0,22,23,0,24,10,0,9,25,11,8,0,7,0,0,5,0,6,12,13,0,19,16,26,20,0,21};

static char const * pin_names[40] = {"3.3 V", "5 V", "GPIO 2", "5 V", "GPIO 3", "GND", "GPIO 4", "UART0 TX", "GND", "UART0 RX", "GPIO 17", "GPIO 18", "GPIO 27", "GND", "GPIO 22", "GPIO 23", "3.3 V", "GPIO 24", "GPIO 10", "GND", "GPIO 9", "GPIO 25", "GPIO 11", "GPIO 8", "GND", "GPIO 7", "RESERVED", "RESERVED", "GPIO 5", "GND", "GPIO 6", "GPIO 12", "GPIO 13", "GND", "GPIO 19", "GPIO 16", "GPIO 26", "GPIO 20", "GND", "GPIO 21"};
static char const * pin_alt_names[40];


void process_pin(uint32_t pin, char * value)
{
	static int init = 0;
	if(!init)
	{
		pin_alt_names[gpio_to_pin[SPI_MOSI - 1] - 1] = "SPI MOSI";
		pin_alt_names[gpio_to_pin[SPI_MISO - 1] - 1] = "SPI MISO";
		pin_alt_names[gpio_to_pin[SPI_SCLK - 1] - 1] = "SPI SERIAL CLOCK";
		pin_alt_names[gpio_to_pin[SPI_CS0 - 1] - 1] = "SPI CHIP SELECT 0";
		pin_alt_names[gpio_to_pin[SPI_CS1 - 1] - 1] = "SPI CHIP SELECT 1";
		pin_alt_names[gpio_to_pin[ADS_RST - 1] - 1] = "ADS RESET";
		pin_alt_names[gpio_to_pin[ADS_CS0 - 1] - 1] = "ADS CHIP SELECT 0";
		pin_alt_names[gpio_to_pin[ADS_CS1 - 1] - 1] = "ADS CHIP SELECT 1";
		pin_alt_names[gpio_to_pin[ADS_DRDY - 1] - 1] = "ADS DATA READY";
		pin_alt_names[gpio_to_pin[ADS_PDWN - 1] - 1] = "ADS POWER DOWN";
		pin_alt_names[gpio_to_pin[OPT_A - 1] - 1] = "OPTO SWITCH A";
		pin_alt_names[gpio_to_pin[OPT_B - 1] - 1] = "OPTO SWITCH B";
		pin_alt_names[gpio_to_pin[AMP_A0 - 1] - 1] = "PGA GAIN 0";
		pin_alt_names[gpio_to_pin[AMP_A1 - 1] - 1] = "PGA GAIN 1";
		pin_alt_names[gpio_to_pin[MOT_M0 - 1] - 1] = "MOTOR M0";
		pin_alt_names[gpio_to_pin[MOT_M0_NE - 1] - 1] = "MOTOR M0 NE";
		pin_alt_names[gpio_to_pin[MOT_M1 - 1] - 1] = "MOTOR M1";
		pin_alt_names[gpio_to_pin[MOT_NSLEEP - 1] - 1] = "MOTOR NSLEEP";
		pin_alt_names[gpio_to_pin[MOT_STEP - 1] - 1] = "MOTOR STEP";
		pin_alt_names[gpio_to_pin[MOT_DIR - 1] - 1] = "MOTOR DIRECTION";
		init = 1;		
	}	

	uint32_t gpio = pin_to_gpio[pin - 1];
	if(gpio && value)
	{
		if(!strcmp(value, "in")) gpio_set_fsel(gpio, IN);
		else if(!strcmp(value, "low"))
		{
			gpio_set_fsel(gpio, OUT);
			gpio_set_level(gpio, LOW);
		}
		else if(!strcmp(value, "high"))
		{
			gpio_set_fsel(gpio, OUT);
			gpio_set_level(gpio, HIGH);
		}
	}
	tcp_server_printf("%02d:%s", pin, pin_names[pin - 1]);
	if(pin_alt_names[pin - 1]) tcp_server_printf(",%s", pin_alt_names[pin - 1]);
	if(gpio)
	{
		uint32_t fsel = gpio_get_fsel(gpio);
		uint32_t level = gpio_get_level(gpio);
		tcp_server_printf(",%s,%s", fsel == IN ? "in" : fsel == OUT ? "out" : "alt", level == LOW ? "low" : "high"); 
	}
	tcp_server_printf("\n");
}

void process_data_rate(char * value)
{
	uint32_t data_rate;
	uint8_t drate;
	if(value && sscanf(value, "%5u", &data_rate) == 1)
	{
		if(data_rate > 22500) drate = SPS_30000;
		else if(data_rate > 11250) drate = SPS_15000;
		else if(data_rate > 5625) drate = SPS_7500;
		else if(data_rate > 2875) drate = SPS_3750;
		else if(data_rate > 1500) drate = SPS_2000;
		else if(data_rate > 750) drate = SPS_1000;
		else if(data_rate > 300) drate = SPS_500;
		else if(data_rate > 80) drate = SPS_100;
		else if(data_rate > 55) drate = SPS_60;
		else if(data_rate > 40) drate = SPS_50;
		else if(data_rate > 27.5) drate = SPS_30;
		else if(data_rate > 20) drate = SPS_25;
		else if(data_rate > 12.5) drate = SPS_15;
		else if(data_rate > 7.5) drate = SPS_10;
		else if(data_rate > 3.75) drate = SPS_5;
		else drate = SPS_2_5;
		ads_set_data_rate(drate);
	}
	drate = ads_get_data_rate();
	tcp_server_printf("data-rate:%s\n", drate == SPS_30000 ? "30000" :
		drate == SPS_15000 ? "15000" :
		drate == SPS_7500 ? "7500" :
		drate == SPS_3750 ? "3750" :
		drate == SPS_2000 ? "2000" :
		drate == SPS_1000 ? "1000" :
		drate == SPS_500 ? "500" :
		drate == SPS_100 ? "100" :
		drate == SPS_60 ? "60" :
		drate == SPS_50 ? "50" :
		drate == SPS_30 ? "30" :
		drate == SPS_25 ? "25" :
		drate == SPS_15 ? "15" :
		drate == SPS_10 ? "10" :
		drate == SPS_5 ? "5" : "2.5");
}

void process_channel(char * value)
{
	uint8_t channel;
	if(value && sscanf(value, "%2hhx", &channel) == 1)
	{
		if(channel < 0x70 && (channel & 0x0F) < 0x07) ads_set_channel(channel);
	}
	channel = ads_get_channel();
	tcp_server_printf("channel:%02x\n", channel);
}

void process_data(char * value)
{
	(void) value;
	tcp_server_printf("data:%d\n", ads_read_data());
}

void process_a(char * value)
{
	if(value)
	{
		if(!strcmp(value, "low")) opto_set_a(LOW);
		else if(!strcmp(value, "high")) opto_set_a(HIGH);
	}
	uint32_t level = opto_get_a();
	if(level == LOW) tcp_server_printf("a:low\n");
	else tcp_server_printf("a:%d\n", opto_read_a());
}

void process_b(char * value)
{
	if(value)
	{
		if(!strcmp(value, "low")) opto_set_b(LOW);
		else if(!strcmp(value, "high")) opto_set_b(HIGH);
	}
	uint32_t level = opto_get_b();
	if(level == LOW) tcp_server_printf("b:low\n");
	else tcp_server_printf("b:%d\n", opto_read_b());
}

void process_active(char * value)
{
	if(value)
	{
		if(!strcmp(value, "low")) motor_set_active(LOW);
		else if(!strcmp(value, "high")) motor_set_active(HIGH);
	}
	uint32_t level = motor_get_active();
	tcp_server_printf("active:%s\n", level == LOW ? "low" : "high");
}

void process_direction(char * value)
{
	if(value)
	{
		if(!strcmp(value, "red-blue")) motor_set_direction(RED_TO_BLUE);
		else if(!strcmp(value, "blue-red")) motor_set_direction(BLUE_TO_RED);
	}
	uint32_t direction = motor_get_direction();
	tcp_server_printf("direction:%s\n", direction == RED_TO_BLUE ? "red-blue" : "blue-red");
}

void process_step_divider(char * value)
{
	uint32_t divider;
	if(value && sscanf(value, "%2u", &divider) == 1)
	{
		if(divider > 24) divider = DIV_32;
		else if(divider > 12) divider = DIV_16;
		else if(divider > 6) divider = DIV_8;
		else if(divider > 3) divider = DIV_4;
		else if(divider > 1.5) divider = DIV_2;
		else divider = DIV_1;
		motor_set_step_divider(divider);
	}
	divider = motor_get_step_divider();
	tcp_server_printf("step-divider:%s\n", divider == DIV_1 ? "1" :
		divider == DIV_2 ? "2" :
		divider == DIV_4 ? "4" :
		divider == DIV_8 ? "8" :
		divider == DIV_16 ? "16" : "32");
}

void process_steps(char * value)
{
	uint32_t steps;
	if(value && sscanf(value, "%5u", &steps) == 1 && steps < 16000)
	{
		for(uint32_t i = 0; i < steps; ++i)
		{
			motor_step();
		}
	}
}

void process_gain(char * value)
{
	uint32_t gain;
	if(value && sscanf(value, "%4u", &gain) == 1)
	{
		if(gain > 550) gain = GAIN_1000;
		else if(gain > 55) gain = GAIN_100;
		else if(gain > 5.5) gain = GAIN_10;
		else gain = GAIN_1;
		amp_set_gain(gain);
	}
	gain = amp_get_gain();
	tcp_server_printf("gain:%s\n", gain == GAIN_1 ? "1" :
		gain == GAIN_10 ? "10" :
		gain == GAIN_100 ? "100" : "1000");
}

void process_offset(char * value)
{
	motor_set_active(HIGH);
	motor_set_direction(BLUE_TO_RED);

	opto_set_a(HIGH);
	while(opto_read_a() < 20000) motor_step();
	opto_set_a(LOW);

	opto_set_b(HIGH);
	while(opto_read_b() < 20000) motor_step();
	opto_set_b(LOW);
        
	uint32_t offset;
        if(value && sscanf(value, "%5u", &offset) == 1)
        {

                if(offset > 0) motor_set_direction(RED_TO_BLUE);
                for(uint32_t i = 0; i < offset; ++i) motor_step();
                tcp_server_printf("offset:%u\n", offset);

        }
	
	motor_set_active(LOW);
}

void process_count(char * value)
{
	motor_set_active(HIGH);
	motor_set_direction(RED_TO_BLUE);

	ads_set_channel(AIN6 << 4 | AIN5);
	tcp_server_printf("data:%d", ads_read_data());
	motor_step();
	
	uint32_t count;
        if(value && sscanf(value, "%5u", &count) == 1 && count > 0)
        {

                for(uint32_t i = 1; i < count; ++i)
                {
                        tcp_server_printf(",%d", ads_read_data());
			motor_step();
                }
        }

	tcp_server_printf("\n");
	
	motor_set_active(LOW);
}
