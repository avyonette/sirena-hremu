// NOTE: this uart construct is used both for the CPU and the emulator side so it's a bit more complicated than it would need to be on real hardware
#include "uart.h"
#include "cpu/cu.h"
#include "cpu/int.h"
#include "emu.h"
#include "hw/gpio.h"
#include "hw/pit.h"
#include "log.h"

Uart uart[UART_COUNT] = {0};
usize uart0_clock = 1;

void uart_configure(Uart* uart, u32 baud_rate, UartParityBit parity) {
	uart->parity = parity;
	uart->rx_pin = UART_RX;
	uart->tx_pin = UART_TX;
	uart->rx_int = INT_UART_RX;
	uart->rx_callback = NULL;

	pit_timers[UART_PIT].target_int = INT_UART_TICK;
	pit_timers[UART_PIT].tick_rate = baud_rate;
	pit_timers[UART_PIT].mode = PIT_TICK;

	CPU_DBG(
		"UART 0 configured (%d baud, parity %s)",
		baud_rate,
		parity == UART_PARITY_NONE ? "none" :
			(parity == UART_PARITY_EVEN ? "even" : "odd")
	);
}

void uart_tick_rx(Uart* uart) {
	bool current_pin = gpio_digital_read(uart->rx_pin);

	switch(uart->rx_state) {
		case UART_RX_IDLE: {
			// falling edge detection (HIGH -> LOW)
			if(uart->rx_prev_pin == GP_HIGH && current_pin == GP_LOW) {
				uart->rx_state = UART_RX_DATA;
				uart->rx_data = 0;
				uart->rx_bits_received = 0;
			}
			break;
		}
		case UART_RX_DATA: {
			uart->rx_data |= (current_pin ? 1 : 0) << uart->rx_bits_received;
			uart->rx_bits_received++;

			if(uart->rx_bits_received == 8) {
				uart->rx_state = UART_RX_IDLE;
				
				if(uart->rx_int > 0) {
					cu_raise_int(INT_UART_RX, uart->rx_data, 0);
				}

				if(uart->rx_callback != NULL) {
					uart->rx_callback(uart->rx_data);
				}
			}
			break;
		}
	}

	uart->rx_prev_pin = current_pin;
}

void uart_tick_tx(Uart* uart) {
	switch(uart->tx_state) {
		case UART_TX_IDLE: {
			gpio_digital_write(uart->tx_pin, UART_IDLE);
			break;
		}
		case UART_TX_START: {
			gpio_digital_write(uart->tx_pin, UART_START_BIT);
			uart->tx_bits_remaining = 8;
			uart->tx_state = UART_TX_DATA;
			uart->_tx_data = uart->tx_data;
			break;
		}
		case UART_TX_DATA: {
			bool data_bit = uart->tx_data & 1;
			gpio_digital_write(uart->tx_pin, data_bit);
			uart->tx_data >>= 1;

			if(--uart->tx_bits_remaining == 0) {
				uart->tx_state = UART_TX_STOP;
			}
			break;
		}
		case UART_TX_STOP: {
			gpio_digital_write(uart->tx_pin, UART_STOP_BIT);
			uart->tx_state = UART_TX_IDLE;

			emu_uart_rx(uart->_tx_data);
			break;
		}
	}
}

word uart_io_read(Uart* uart, p_addr address) {
	switch(address) {
		case 4: {
			if(uart->rx_bits_received == 8) {
				return uart->rx_data;
			}

			return 0;
		}
		case 6:
			return uart->tx_state;
		case 7:
			return uart->rx_state;
	}

	return 0;
}

void uart_io_write(Uart* uart, p_addr address, word data) {
	switch(address) {
		case 0: {
			UartConfigurePacket* cfg = (UartConfigurePacket*) &data;

			if(cfg->pr > UART_PARITY_EVEN) {
				cfg->pr = UART_PARITY_ODD;
			}

			if(cfg->bd > CLK_MAIN_FREQ / 2 /*|| cfg->bd < 9600*/) {
				cfg->bd = 9600;
			}

			uart_configure(uart, cfg->bd, cfg->pr);
			break;
		}
		case 4: {
			if(uart->tx_state != UART_TX_IDLE) {
				cu_raise_int(INT_IOP, INT_IOP_0_UART_WRITE_DURING_TRANSFER, 0);
				return;
			}

			uart->tx_data = data;
			uart->tx_bits_remaining = 8;
			uart->tx_state = UART_TX_START;

			CPU_DBG("Sending %c through UART 0", uart->tx_data);
			break;
		}
	}
}
