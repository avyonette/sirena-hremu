#pragma once
#include <types.h>
#include <arch_types.h>

typedef struct HardwareMemory {
	byte* ptr;
	u32 size;
	bool rw;
} HardwareMemory;

void mem_init(HardwareMemory* mem, u32 size, bool rw);
void mem_free(HardwareMemory* mem);

byte mem_read8(HardwareMemory* mem, p_addr address);
half_word mem_read16(HardwareMemory* mem, p_addr address);
word mem_read32(HardwareMemory* mem, p_addr address);

void mem_write8(HardwareMemory* mem, p_addr address, byte data);
void mem_write16(HardwareMemory* mem, p_addr address, half_word data);
void mem_write32(HardwareMemory* mem, p_addr address, word data);
