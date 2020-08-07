#include "spectrophotometer.h"

#include "gpio.h"
#include "spi.h"
#include "ads.h"
#include "motor.h"
#include "opto.h"
#include "amp.h"
#include "tcp.h"
#include "args.h"

#include <stdio.h>
#include <string.h>

int spectrophotometer_init()
{
	if(gpio_init()) return -1;
	if(spi_init()) return -1;
	if(ads_init()) return -1;
	if(motor_init()) return -1;
	if(opto_init()) return -1;
	if(amp_init()) return -1;

	return 0;
}

int spectrophotometer_deinit()
{
	amp_deinit();
	opto_deinit();
	motor_deinit();
	ads_deinit();
	spi_deinit();
	gpio_deinit();

	return 0;
}

static void pin_request_handler(char * message)
{
        tcp_server_printf("PIN-RESPONSE\n");
	if(!message[0])
	{
		for(int pin = 1; pin <= 40; ++pin)
		{
			process_pin(pin, NULL);
		}
	}
	else
	{
		for(char * line = strtok_r(NULL, "\n", &message); line; line = strtok_r(NULL, "\n", &message))
		{
			char * name = 0, * value = 0;
			if((name = strtok_r(line, ":", &value)))
			{
				uint32_t pin;
				if(sscanf(name, "%2u", &pin) == 1) process_pin(pin, value);
			}
		}
	}
        tcp_server_printf("\n");
}

static void adc_request_handler(char * message)
{
        tcp_server_printf("ADC-RESPONSE\n");
	if(!message[0])
	{
		process_data_rate(NULL);
		process_channel(NULL);
		process_data(NULL);
	}
	else
	{
		for(char * line = strtok_r(NULL, "\n", &message); line; line = strtok_r(NULL, "\n", &message))
		{
			char * name = 0, * value = 0;
			if((name = strtok_r(line, ":", &value)))
			{
				if(!strcmp(name, "data-rate")) process_data_rate(value);
				else if(!strcmp(name, "channel")) process_channel(value);
				else if(!strcmp(name, "data")) process_data(value);
			}
		}
	}
        tcp_server_printf("\n");
}

static void opto_request_handler(char * message)
{
        tcp_server_printf("OPTO-RESPONSE\n");
        if(!message[0])
	{
		process_a(NULL);
		process_b(NULL);
	}
	else
	{	
		for(char * line = strtok_r(NULL, "\n", &message); line; line = strtok_r(NULL, "\n", &message))
		{
			char * name, * value;
			if((name = strtok_r(line, ":", &value)))
			{
				if(!strcmp(name, "a")) process_a(value);
				else if(!strcmp(name, "b")) process_b(value);
			}
		}
	}
        tcp_server_printf("\n");
}

static void motor_request_handler(char * message)
{
        tcp_server_printf("MOTOR-RESPONSE\n");
	if(!message[0])
	{
		process_active(NULL);
		process_direction(NULL);
		process_step_divider(NULL);
	}
	else
	{
		for(char * line = strtok_r(NULL, "\n", &message); line; line = strtok_r(NULL, "\n", &message))
		{
			char * name, * value;
			if((name = strtok_r(line, ":", &value)))
			{
				if(!strcmp(name, "active")) process_active(value);
				else if(!strcmp(name, "direction")) process_direction(value);
				else if(!strcmp(name, "step-divider")) process_step_divider(value);
				else if(!strcmp(name, "steps")) process_steps(value);
			}
		}
	}
        tcp_server_printf("\n");
}

static void amp_request_handler(char * message)
{
        tcp_server_printf("AMP-RESPONSE\n");
	if(!message[0])
	{
		process_gain(NULL);
	}
	else
	{
		for(char * line = strtok_r(NULL, "\n", &message); line; line = strtok_r(NULL, "\n", &message))
		{
			char * name, * value;
			if((name = strtok_r(line, ":", &value)))
			{
				if(!strcmp(name, "gain")) process_gain(value);
			}
		}
	}
        tcp_server_printf("\n");
}

static void position_request_handler(char * message)
{
        tcp_server_printf("POSITION-RESPONSE\n");
	if(!message[0])
	{
		process_offset(NULL);
	}
	else
	{
		for(char * line = strtok_r(NULL, "\n", &message); line; line = strtok_r(NULL, "\n", &message))
		{
			char * name, * value;
			if((name = strtok_r(line, ":", &value)))
			{
				if(!strcmp(name, "offset")) process_offset(value);
			}
		}
	}
        tcp_server_printf("\n");
}

static void sample_request_handler(char * message)
{
        tcp_server_printf("SAMPLE-RESPONSE\n");
	if(!message[0])
	{
		process_count(NULL);
	}
	else
	{
		for(char * line = strtok_r(NULL, "\n", &message); line; line = strtok_r(NULL, "\n", &message))
		{
			char * name, * value;
			if((name = strtok_r(line, ":", &value)))
			{
				if(!strcmp(name, "count")) process_count(value);
			}
		}
	}
	tcp_server_printf("\n");
}

void spectrophotometer_message_handler(char * message, int length)
{
        message[length - 1] = 0;
        char * header = strtok_r(message, "\n", &message);
        if(header)
        {
                if(!strcmp(header, "PIN-REQUEST")) pin_request_handler(message);
                else if(!strcmp(header, "ADC-REQUEST")) adc_request_handler(message);
                else if(!strcmp(header, "MOTOR-REQUEST")) motor_request_handler(message);
                else if(!strcmp(header, "OPTO-REQUEST")) opto_request_handler(message);
                else if(!strcmp(header, "AMP-REQUEST")) amp_request_handler(message);
                else if(!strcmp(header, "POSITION-REQUEST")) position_request_handler(message);
                else if(!strcmp(header, "SAMPLE-REQUEST")) sample_request_handler(message);
        }
}
