#include <zephyr.h>
#include <misc/printk.h>
#include <gpio.h>
#include <device.h>
#include <stdio.h>

#include "rgb_led.h"

void main(void)
{
	rgb_led_init();

	rgb_led_set(0xff,0,0); // Red to indicate 'starting up'
}
