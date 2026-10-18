#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlog.h>
#include <assert.h>
#include <argtable3.h>
#include "dbg/dbg.h"
#include "dbg/disasm.h"
#include "log.h"
#include "mem/mem.h"
#include "mem/ram.h"
#include "mem/rom.h"
#include "cpu/cu.h"
#include "opts.h"

zlog_category_t* lc_emu;
zlog_category_t* lc_cpu;

HardwareMemory user_ram;
HardwareMemory system_ram;
HardwareMemory blip_rom;

u64 cycle_count = 0;

bool dump_regs_after_cycle = false;
bool auto_disassemble = false;

struct arg_file* cli_blip;
struct arg_lit* cli_auto_dump_regs;
struct arg_lit* cli_auto_disasm;
struct arg_end* cli_end;

//#define ITERATIONS 50

int main(int argc, char* argv[]) {
	void* argt[] = {
		cli_blip = arg_filen(NULL, "blip", "<file>", 1, 1, "BLIP rom file"),
		cli_auto_dump_regs = arg_litn("r", "dump-regs", 0, 1, "automatically dump registers by the end of each cycle"),
		cli_auto_disasm = arg_litn("d", "auto-disasm", 0, 1, "automatically disassemble the RIP at the beginning of each cycle"),
		cli_end = arg_end(1)
	};

	zlog_init("log.conf");
	
	lc_emu = zlog_get_category("emu");
	assert(lc_emu);

	lc_cpu = zlog_get_category("cpu");
	assert(lc_cpu);

	if(arg_parse(argc, argv, argt) > 0) {
		printf("Usage:\n");
		arg_print_syntax(stdout, argt, "\n");
		goto exit;
	} else {
		dump_regs_after_cycle = cli_auto_dump_regs->count > 0;
		auto_disassemble = cli_auto_disasm->count > 0;
	}

	zlog_info(lc_emu, "BLIP ROM file name: %s", cli_blip->filename[0]);

	zlog_info(lc_emu, "initialising memory");
	mem_init(&user_ram, USER_RAM_SIZE, true);
	mem_init(&system_ram, SYSTEM_RAM_SIZE, true);
	mem_init(&blip_rom, BLIP_ROM_SIZE, false);

	zlog_info(lc_emu, "user RAM size: %#010x", user_ram.size);
	zlog_info(lc_emu, "system RAM size: %#010x", system_ram.size);
	zlog_info(lc_emu, "BLIP ROM size: %#010x", blip_rom.size);

	{
		zlog_info(lc_emu, "reading BLIP ROM data from file");

		FILE* rom = fopen(cli_blip->filename[0], "r");
		assert(rom);

		fseek(rom, 0, SEEK_END);
		usize rom_size = ftell(rom);
		fseek(rom, 0, SEEK_SET);

		void* rom_data = malloc(rom_size);
		fread(rom_data, 1, rom_size, rom);
		fclose(rom);

		zlog_info(lc_emu, "writing BLIP ROM data to memory");
		memcpy(blip_rom.ptr, rom_data, rom_size);
	}

	zlog_info(lc_emu, "cpu init");
	cu_init();

#ifdef ITERATIONS
	for(int i = 0; i < ITERATIONS; i++) {
#else
	while(true) {
#endif

		if(auto_disassemble) {
			word inst_data = 0;
			dbg_cpu_get_rip_data32(&inst_data);

			DebugDasmInstruction inst;
			dbg_disasm_init(&inst);
			dbg_disasm_inst(&inst, inst_data);

			zlog_info(lc_emu, "-- %08X: %08X %s %s%s %s%s %s",
				cu_read_reg(REG_RIP), inst_data,
				inst.name,
				inst.param0,
				inst.param1[0] == '\0' ? "" : ",", inst.param1,
				inst.param2[0] == '\0' ? "" : ",", inst.param2
			);

			dbg_disasm_free(&inst);
		}

		cu_step();

		if(dump_regs_after_cycle) {
			printf("cycle %lu reg dump:\n", cycle_count);
			dbg_cpu_dump_regs(stdout);
			printf("\n");
		}

		cycle_count++;
	}

	zlog_info(lc_emu, "\nGOODBYE :)");
	zlog_info(lc_emu, "Total cycles: %lu\n", cycle_count);
	dbg_cpu_dump_regs(stdout);

exit: ;
	arg_freetable(argt, sizeof(argt) / sizeof(argt[0]));
	return 0;
}
