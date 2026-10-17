#include <zlog.h>
#include <assert.h>
#include "log.h"
#include "mem/mem.h"
#include "mem/ram.h"
#include "mem/rom.h"

zlog_category_t* lc_emu;

HardwareMemory user_ram;
HardwareMemory system_ram;
HardwareMemory blip_rom;

int main() {
	zlog_init("log.conf");
	
	lc_emu = zlog_get_category("hremu");
	assert(lc_emu);

	zlog_info(lc_emu, "initialising memory");
	mem_init(&user_ram, USER_RAM_SIZE, true);
	mem_init(&system_ram, SYSTEM_RAM_SIZE, true);
	mem_init(&blip_rom, BLIP_ROM_SIZE, false);

	zlog_info(lc_emu, "user RAM size: %#010x", user_ram.size);
	zlog_info(lc_emu, "system RAM size: %#010x", system_ram.size);
	zlog_info(lc_emu, "BLIP ROM size: %#010x", blip_rom.size);

	return 0;
}
