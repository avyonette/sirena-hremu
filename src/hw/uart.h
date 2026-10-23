#pragma once
#include "types.h"
#include "gpio.h"

#define UART_COUNT 1
#define UART_PIT 0

#define UART_RX GP_0
#define UART_TX GP_1

#define UART_IDLE GP_HIGH
#define UART_START_BIT GP_LOW
#define UART_STOP_BIT GP_HIGH

#define INT_UART_TICK 0x90
#define INT_UART_RX 0x91

typedef enum UartParityBit {
	UART_PARITY_NONE,
	UART_PARITY_ODD,
	UART_PARITY_EVEN
} UartParityBit;

typedef enum UartRxState {
	UART_RX_IDLE,
	UART_RX_DATA
} UartRxState;

typedef enum UartTxState {
	UART_TX_IDLE,
	UART_TX_START,
	UART_TX_DATA,
	UART_TX_STOP
} UartTxState;

typedef struct Uart {
	UartParityBit parity;

	u8 rx_pin;
	u8 tx_pin;

	u8 rx_int;
	void (*rx_callback)(u8);

	UartRxState rx_state;
	u8 rx_data;
	bool rx_prev_pin;
	u8 rx_bits_received;

	UartTxState tx_state;
	u8 tx_data;
	u8 _tx_data;
	u8 tx_bits_remaining;
} Uart;

typedef struct UartConfigurePacket {
	u32 bd : 24;
	UartParityBit pr : 3;
	u8 _reserved_0 : 5;
} UartConfigurePacket;

extern Uart uart[UART_COUNT];

//void uart_configure(Uart* uart, u32 baud_rate, UartParityBit parity);

void uart_tick_rx(Uart* uart);
void uart_tick_tx(Uart* uart);

word uart_io_read(Uart* uart, p_addr address);
void uart_io_write(Uart* uart, p_addr address, word data);
