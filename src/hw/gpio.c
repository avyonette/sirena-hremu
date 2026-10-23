#include "gpio.h"

float gpio_data[GP_COUNT] = {0};

bool gpio_digital_read(u8 pin) {
	assert(pin < GP_COUNT);
	return gpio_data[pin] > GPIO_DIGITAL_THRESHOLD;
}

void gpio_digital_write(u8 pin, bool value) {
	assert(pin < GP_COUNT);
	gpio_data[pin] = value ? GP_HIGH_REF : GP_LOW_REF;
}
