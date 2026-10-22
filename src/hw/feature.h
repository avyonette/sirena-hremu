#pragma once
#include "arch_types.h"

typedef enum HardwareFeature {
	FT_PIT = 0x01,
	FT_UART = 0x02,
	FT_SPI = 0x03,
	FT_SPPB = 0x04,

	FT_PIT_BIT = 1 << 0,
	FT_UART_BIT = 1 << 1,
	FT_SPI_BIT = 1 << 2,
	FT_SPPB_BIT = 1 << 3,
} HardwareFeature;

word hwft_get_available_features();
word hwft_get_info(HardwareFeature feature);
