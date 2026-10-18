#include <argtable3.h>
#include <string.h>
#include <stdlib.h>
#include "emu.h"
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

	EMU_INFO("cpu init");
	cu_init();
}

void emu_iter() {
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
