#include "motor.h"

#include "gpio.h"

#include <unistd.h> //usleep

uint32_t const RED_TO_BLUE = 0;
uint32_t const BLUE_TO_RED = 1;

uint32_t const DIV_1  = 0x00000000;
uint32_t const DIV_2  = 0x00000002;
uint32_t const DIV_4  = 0x00000003;
uint32_t const DIV_8  = 0x00000004;
uint32_t const DIV_16 = 0x00000005;
uint32_t const DIV_32 = 0x00000006;

int motor_init()
{
	//initialization cannot be completed without access to GPIOs
        if(gpio_init()) return -1;

        gpio_set_fsel(MOT_M0, OUT);
	gpio_set_fsel(MOT_M0_NE, OUT);
        gpio_set_fsel(MOT_M1, OUT);
        gpio_set_fsel(MOT_NSLEEP, OUT);
        gpio_set_fsel(MOT_STEP, OUT);
        gpio_set_fsel(MOT_DIR, OUT);

        gpio_set_level(MOT_M0, LOW);
	gpio_set_level(MOT_M0_NE, HIGH);
        gpio_set_level(MOT_M1, HIGH);
        gpio_set_level(MOT_NSLEEP, LOW);
        gpio_set_level(MOT_STEP, LOW);
        gpio_set_level(MOT_DIR, RED_TO_BLUE);

	return 0;
}

int motor_deinit()
{
        if(gpio_init()) return -1;

	gpio_set_fsel(MOT_M0_NE, IN);
        gpio_set_fsel(MOT_M0, IN);
        gpio_set_fsel(MOT_M1, IN);
        gpio_set_fsel(MOT_NSLEEP, IN);
        gpio_set_fsel(MOT_STEP, IN);
        gpio_set_fsel(MOT_DIR, IN);

	return 0;
}

uint32_t motor_get_step_divider()
{
	uint32_t M0 = gpio_get_level(MOT_M0);
	uint32_t M0_NE = gpio_get_level(MOT_M0_NE);
	uint32_t M1 = gpio_get_level(MOT_M1);

	return M1 << 2 | M0_NE << 1 | M0;
}

void motor_set_step_divider(uint32_t divider)
{
	gpio_set_level(MOT_M0, divider & 0x00000001 ? HIGH : LOW);
	gpio_set_level(MOT_M0_NE, divider & 0x00000002 ? HIGH : LOW);
	gpio_set_level(MOT_M1, divider & 0x00000004 ? HIGH : LOW);
}

uint32_t motor_get_direction()
{
	return gpio_get_level(MOT_DIR);
}

void motor_set_direction(uint32_t direction)
{
	gpio_set_level(MOT_DIR, direction);
}

uint32_t motor_get_active()
{
	return gpio_get_level(MOT_NSLEEP);
}

void motor_set_active(uint32_t active)
{
	gpio_set_level(MOT_NSLEEP, active);
	usleep(2000); //theoretically up to 1 ms
}

void motor_step()
{
	for(int i = 0; i < 32; ++i)
	{
		gpio_set_level(MOT_STEP, HIGH);
		usleep(5); //theoretically up to 1.9 us
		gpio_set_level(MOT_STEP, LOW);
		usleep(5); //theoretically up to 1.9 us
		usleep(100); //give time for motor to move
	}
}
