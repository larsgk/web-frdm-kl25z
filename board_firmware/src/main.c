#include <zephyr.h>
#include <sensor.h>
#include <stdio.h>

#include "rgb_led.h"

K_SEM_DEFINE(sem, 0, 1);	/* starts off "not available" */

static void trigger_handler(struct device *dev, struct sensor_trigger *trigger)
{
	ARG_UNUSED(trigger);

	if (sensor_sample_fetch(dev)) {
		printf("sensor_sample_fetch failed\n");
		return;
	}

	k_sem_give(&sem);
}

void main(void)
{
	struct sensor_value accel[3];
	struct device *dev = device_get_binding(DT_NXP_FXOS8700_0_LABEL);

	if (dev == NULL) {
		printf("Could not get fxos8700 device\n");
		return;
	}

	struct sensor_value attr = {
		.val1 = 6,
		.val2 = 250000,
	};

	// if (val->val1 == 800 && val->val2 == 0) {
	// 	dr = FXOS8700_CTRLREG1_DR_RATE_800;
	// } else if (val->val1 == 400 && val->val2 == 0) {
	// 	dr = FXOS8700_CTRLREG1_DR_RATE_400;
	// } else if (val->val1 == 200 && val->val2 == 0) {
	// 	dr = FXOS8700_CTRLREG1_DR_RATE_200;
	// } else if (val->val1 == 100 && val->val2 == 0) {
	// 	dr = FXOS8700_CTRLREG1_DR_RATE_100;
	// } else if (val->val1 == 50 && val->val2 == 0) {
	// 	dr = FXOS8700_CTRLREG1_DR_RATE_50;
	// } else if (val->val1 == 12 && val->val2 == 500000) {
	// 	dr = FXOS8700_CTRLREG1_DR_RATE_12_5;
	// } else if (val->val1 == 6 && val->val2 == 250000) {
	// 	dr = FXOS8700_CTRLREG1_DR_RATE_6_25;
	// } else if (val->val1 == 1 && val->val2 == 562500) {
	// 	dr = FXOS8700_CTRLREG1_DR_RATE_1_56;
	// } else {
	// 	return -EINVAL;
	// }

	if (sensor_attr_set(dev, SENSOR_CHAN_ALL,
			    SENSOR_ATTR_SAMPLING_FREQUENCY, &attr)) {
		printk("Could not set sampling frequency\n");
		return;
	}

	struct sensor_trigger trig = {
		.type = SENSOR_TRIG_DATA_READY,
		.chan = SENSOR_CHAN_ACCEL_XYZ,
	};

	if (sensor_trigger_set(dev, &trig, trigger_handler)) {
		printf("Could not set trigger\n");
		return;
	}

	while (1) {
		k_sem_take(&sem, K_FOREVER);

		sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ, accel);
		printf("AX=%10.6f AY=%10.6f AZ=%10.6f ",
		       sensor_value_to_double(&accel[0]),
		       sensor_value_to_double(&accel[1]),
		       sensor_value_to_double(&accel[2]));
		printf("\n");
	}


	rgb_led_init();

	rgb_led_set(0xff,0,0); // Red to indicate 'starting up'
}
