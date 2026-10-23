#pragma once
#include "hw/uart.h"
#include "types.h"

#define EMU_UART_BAUD 9600

extern u64 emu_iter_count;
extern bool emu_run;

extern Uart emu_uart;
extern usize emu_uart_clock;

void emu_init();
void emu_iter();

void emu_uart_rx(char c);
void emu_uart_tx(char c);
