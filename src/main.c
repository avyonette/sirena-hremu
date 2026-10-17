#include <string.h>
#include <zlog.h>
#include <assert.h>
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

const int iterations = 5;

int main() {
	zlog_init("log.conf");
	
	lc_emu = zlog_get_category("emu");
	assert(lc_emu);

	lc_cpu = zlog_get_category("cpu");
	assert(lc_cpu);

	zlog_info(lc_emu, "initialising memory");
	mem_init(&user_ram, USER_RAM_SIZE, true);
	mem_init(&system_ram, SYSTEM_RAM_SIZE, true);
	mem_init(&blip_rom, BLIP_ROM_SIZE, false);

	zlog_info(lc_emu, "user RAM size: %#010x", user_ram.size);
	zlog_info(lc_emu, "system RAM size: %#010x", system_ram.size);
	zlog_info(lc_emu, "BLIP ROM size: %#010x", blip_rom.size);

	zlog_info(lc_emu, "writing BLIP data to BLIP ROM");
	memcpy(blip_rom.ptr, _blip_rom_data, sizeof(_blip_rom_data) / sizeof(_blip_rom_data[0]));

	zlog_info(lc_emu, "cpu init");
	cu_init();

	for(int i = 0; i < iterations; i++) {
		cu_step();

		zlog_info(lc_emu, "== REGISTER DUMP BEGIN ==");
		dbg_cpu_dump_regs(stdout);
		zlog_info(lc_emu, "==  REGISTER DUMP END  ==");
	}

	zlog_info(lc_emu, "goodbye");

	return 0;
}
