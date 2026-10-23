#include "uart.h"
#include "cpu/cu.h"
#include "cpu/int.h"
#include "hw/clk.h"
#include "hw/gpio.h"
#include "hw/pit.h"
#include "log.h"

Uart uart0 = {0};
usize uart0_clock = 1;

void uart_clock_timeout() {
	uart_tick_rx();
	uart_tick_tx();
}

void uart_configure(u32 baud_rate, UartParityBit parity) {
	assert(parity == UART_PARITY_NONE); // TODO
	clk_create(uart0_clock, baud_rate, uart_clock_timeout, true); // PIT..?

	CPU_DBG(
		"UART 0 configured (%d baud, parity %s)",
		baud_rate,
		parity == UART_PARITY_NONE ? "none" :
			(parity == UART_PARITY_EVEN ? "even" : "odd")
	);
}

void uart_tick_rx() {
	bool current_pin = gpio_digital_read(UART_RX);

	switch(uart0.rx_state) {
		case UART_RX_IDLE: {
			// falling edge detection (HIGH -> LOW)
			if(uart0.rx_prev_pin == GP_HIGH && current_pin == GP_LOW) {
				uart0.rx_state = UART_RX_DATA;
				uart0.rx_data = 0;
				uart0.rx_bits_received = 0;
			}
			break;
		}
		case UART_RX_DATA: {
			uart0.rx_data |= (current_pin ? 1 : 0) << uart0.rx_bits_received;
			uart0.rx_bits_received++;

			if(uart0.rx_bits_received == 8) {
				uart0.rx_state = UART_RX_IDLE;
				cu_raise_int(INT_UART_RX, uart0.rx_data, 0);
			}
			break;
		}
	}

	uart0.rx_prev_pin = current_pin;
}

void uart_tick_tx() {
	switch(uart0.tx_state) {
		case UART_TX_IDLE: {
			gpio_digital_write(UART_TX, UART_IDLE);
			break;
		}
		case UART_TX_START: {
			gpio_digital_write(UART_TX, UART_START_BIT);
			uart0.tx_bits_remaining = 8;
			uart0.tx_state = UART_TX_DATA;
			break;
		}
		case UART_TX_DATA: {
			bool data_bit = uart0.tx_data & 1;
			gpio_digital_write(UART_TX, data_bit);
			uart0.tx_data >>= 1;

			if(--uart0.tx_bits_remaining == 0) {
				uart0.tx_state = UART_TX_STOP;
			}
			break;
		}
		case UART_TX_STOP: {
			gpio_digital_write(UART_TX, UART_STOP_BIT);
			uart0.tx_state = UART_TX_IDLE;
			break;
		}
	}
}

word uart_io_read(p_addr address) {
	//if(address != 0) return 0;
	
	switch(address) {
		case 4: {
			if(uart0.rx_bits_received == 8) {
				return uart0.rx_data;
			}

			return 0;
		}
		case 6:
			return uart0.tx_state;
		case 7:
			return uart0.rx_state;
	}

	return 0;
}

void uart_io_write(p_addr address, word data) {
	switch(address) {
		case 0: {
			UartConfigurePacket* cfg = (UartConfigurePacket*) &data;

			if(cfg->pr > UART_PARITY_EVEN) {
				cfg->pr = UART_PARITY_ODD;
			}

			if(cfg->bd > CLK_MAIN_FREQ / 2 || cfg->bd < 9600) {
				cfg->bd = 9600;
			}

			uart_configure(cfg->bd, cfg->pr);
			break;
		}
		case 4: {
			if(uart0.tx_state != UART_TX_IDLE) {
				cu_raise_int(INT_IOP, INT_IOP_0_UART_WRITE_DURING_TRANSFER, 0);
				return;
			}

			uart0.tx_data = data;
			uart0.tx_bits_remaining = 8;
			uart0.tx_state = UART_TX_START;

			CPU_DBG("Sending %c through UART 0", uart0.tx_data);
			break;
		}
	}
}
