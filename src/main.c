#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlog.h>
#include <assert.h>
#include <argtable3.h>
#include "dbg/dbg.h"
#include "log.h"
#include "mem/mem.h"
#include "mem/ram.h"
#include "mem/rom.h"
#include "cpu/cu.h"

zlog_category_t* lc_emu;
zlog_category_t* lc_cpu;

HardwareMemory user_ram;
HardwareMemory system_ram;
HardwareMemory blip_rom;

struct arg_file* cli_blip;
struct arg_end* cli_end;

#define ITERATIONS 200

int main(int argc, char* argv[]) {
	void* argt[] = {
		cli_blip = arg_filen(NULL, "blip", "<file>", 1, 1, "BLIP rom file"),
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
		cu_step();

		dbg_cpu_dump_regs(stdout);
	}

	zlog_info(lc_emu, "goodbye");

exit: ;
	arg_freetable(argt, sizeof(argt) / sizeof(argt[0]));
	return 0;
}
