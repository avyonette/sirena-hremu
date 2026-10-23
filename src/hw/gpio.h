#pragma once
#include "types.h"
#include "arch_types.h"

#define GPIO_LOGIC_LEVEL 3.3
#define GPIO_DIGITAL_THRESHOLD 1.5

#define GP_HIGH 1
#define GP_LOW 0

#define GP_HIGH_REF GPIO_LOGIC_LEVEL
#define GP_LOW_REF 0.0

#define GP_0 0x00
#define GP_1 0x01
#define GP_2 0x02
#define GP_3 0x03
#define GP_4 0x04
#define GP_5 0x05
#define GP_6 0x06
#define GP_7 0x07
#define GP_8 0x08
#define GP_9 0x09
//#define GP_10 0x0A GPIO_11 is +5V
//#define GP_11 0x0B GPIO_12 is GND
#define GP_12 0x0C
#define GP_13 0x0D
#define GP_14 0x0E
#define GP_15 0x0F
#define GP_16 0x10
#define GP_17 0x11
#define GP_18 0x12
#define GP_19 0x13
#define GP_20 0x14
#define GP_21 0x15
#define GP_22 0x16
#define GP_23 0x17
#define GP_24 0x18
#define GP_25 0x19
#define GP_26 0x1A
#define GP_27 0x1B
//#define GP_28 0x1C GPIO_29 is +3.3V
//#define GP_29 0x1D GPIO_30 is GND
#define GP_30 0x1E
#define GP_31 0x1F
#define GP_32 0x20
#define GP_33 0x21
#define GP_34 0x22
#define GP_35 0x23
#define GP_36 0x24
#define GP_37 0x25
#define GP_38 0x26
#define GP_39 0x27
#define GP_COUNT 40

extern float gpio_data[GP_COUNT];

bool gpio_digital_read(u8 pin);
void gpio_digital_write(u8 pin, bool value);
