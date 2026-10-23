#include <argtable3.h>
#include <string.h>
#include <stdlib.h>
#include "emu.h"
#include "gui/virt_mon.h"
#include "hw/clk.h"
#include "hw/uart.h"
#include "opts.h"
#include "log.h"
#include "mem/mem.h"
#include "mem/ram.h"
#include "mem/rom.h"
#include "cpu/cu.h"
#include "dbg/dbg.h"
#include "dbg/disasm.h"

HardwareMemory user_ram;
HardwareMemory system_ram;
HardwareMemory blip_rom;

u64 emu_iter_count = 0;
bool emu_run = false;

Uart emu_uart = {0};
usize emu_uart_clock = 1;

void emu_uart_clock_callback() {
	if(!emu_run) return;

	uart_tick_tx(&emu_uart);
	uart_tick_rx(&emu_uart);
}

void emu_default_uart_callback(u8 data) {
	printf("UART0: %c\n", data);
}

void emu_init() {
	EMU_INFO("BLIP ROM file name: %s", cli_blip->filename[0]);

	EMU_INFO("initialising memory");
	mem_init(&user_ram, USER_RAM_SIZE, true);
	mem_init(&system_ram, SYSTEM_RAM_SIZE, true);
	mem_init(&blip_rom, BLIP_ROM_SIZE, false);

	EMU_INFO("user RAM size: %#010x", user_ram.size);
	EMU_INFO("system RAM size: %#010x", system_ram.size);
	EMU_INFO("BLIP ROM size: %#010x", blip_rom.size);

	{
		EMU_INFO("reading BLIP ROM data from file");

		FILE* rom = fopen(cli_blip->filename[0], "r");
		assert(rom);

		fseek(rom, 0, SEEK_END);
		usize rom_size = ftell(rom);
		fseek(rom, 0, SEEK_SET);

		void* rom_data = malloc(rom_size);
		fread(rom_data, 1, rom_size, rom);
		fclose(rom);

		EMU_INFO("writing BLIP ROM data to memory");
		memcpy(blip_rom.ptr, rom_data, rom_size);
	}

	/*EMU_INFO("initialising UART and UART clock");
	emu_uart.parity = UART_PARITY_NONE;
	emu_uart.oversampling_counter = 0;
	emu_uart.rx_pin = UART_TX;
	emu_uart.tx_pin = UART_RX;
	emu_uart.rx_int = 0;
	emu_uart.rx_callback = emu_default_uart_callback;
	clk_create(emu_uart_clock, EMU_UART_BAUD * UART_OVERSAMPLING, emu_uart_clock_callback, true);*/

	EMU_INFO("cpu init");
	cu_init();

	EMU_INFO("cpu reset");
	cu_reset();
}

void emu_iter() {
	if(!emu_run) return;

	if(opt_auto_disassemble) {
		word inst_data = 0;
		dbg_cpu_get_rip_data32(&inst_data);

		DebugDasmInstruction inst;
		dbg_disasm_init(&inst);
		dbg_disasm_inst(&inst, inst_data);

		EMU_INFO("-- %08X: %08X %s %s%s %s%s %s",
			cu_read_reg(REG_RIP), inst_data,
			inst.name,
			inst.param0,
			inst.param1[0] == '\0' ? "" : ",", inst.param1,
			inst.param2[0] == '\0' ? "" : ",", inst.param2
		);

		dbg_disasm_free(&inst);
	}

	cu_step();

	if(opt_dump_regs_after_cycle) {
		printf("cycle %lu reg dump:\n", emu_iter_count);
		dbg_cpu_dump_regs(stdout);
		printf("\n");
	}

	emu_iter_count++;
}

void emu_uart_rx(char c) {
	if(opt_headless) printf("%c", c);
	else gui_virt_mon_uart_rx(c);
}

void emu_uart_tx(char c) {
	uart[0].rx_data = c;
	uart[0].rx_bits_received = 8;

	cu_raise_int(INT_UART_RX, uart->rx_data, 0);
}
