#include "mem.h"
#include <stdlib.h>
#include <string.h>

void mem_init(HardwareMemory* mem, u32 size, bool rw) {
	mem->ptr = malloc(size);
	mem->size = size;
	mem->rw = rw;

	//memset(mem->ptr, 0, size);
}

void mem_free(HardwareMemory* mem) {
	free(mem->ptr);
}

byte mem_read8(HardwareMemory* mem, p_addr address) {
	assert(address < mem->size);
	return mem->ptr[address];
}

half_word mem_read16(HardwareMemory* mem, p_addr address) {
	assert(address + 1 < mem->size);
	
	half_word data = 0;
	data |= mem_read8(mem, address);
	data |= mem_read8(mem, address + 1) << 8;

	return data;
}

word mem_read32(HardwareMemory* mem, p_addr address) {
	assert(address + 3 < mem->size);
	
	word data = 0;
	data |= mem_read8(mem, address);
	data |= mem_read8(mem, address + 1) << 8;
	data |= mem_read8(mem, address + 2) << 16;
	data |= mem_read8(mem, address + 3) << 24;

	return data;
}

void mem_write8(HardwareMemory* mem, p_addr address, byte data) {
	assert(mem->rw);
	assert(address < mem->size);
	
	mem->ptr[address] = data;
}

void mem_write16(HardwareMemory* mem, p_addr address, half_word data) {
	assert(mem->rw);
	assert(address + 1 < mem->size);

	mem->ptr[address] = data & 0xFF;
	mem->ptr[address + 1] = data >> 8;
}

void mem_write32(HardwareMemory* mem, p_addr address, word data) {
	assert(mem->rw);
	assert(address + 3 < mem->size);

	mem->ptr[address] = data & 0xFF;
	mem->ptr[address + 1] = (data >> 8) & 0xFF;
	mem->ptr[address + 2] = (data >> 16) & 0xFF;
	mem->ptr[address + 3] = (data >> 24) & 0xFF;
}
