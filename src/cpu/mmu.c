#include <string.h>
#include "mmu.h"
#include "cu.h"
#include "hw/feature.h"
#include "hw/pit.h"
#include "hw/uart.h"
#include "paging.h"
#include "mem/ram.h"
#include "mem/rom.h"

typedef byte (*MemRead8Fn)(HardwareMemory*, p_addr);
typedef half_word (*MemRead16Fn)(HardwareMemory*, p_addr);
typedef word (*MemRead32Fn)(HardwareMemory*, p_addr);

typedef void (*MemWrite8Fn)(HardwareMemory*, p_addr, byte);
typedef void (*MemWrite16Fn)(HardwareMemory*, p_addr, half_word);
typedef void (*MemWrite32Fn)(HardwareMemory*, p_addr, word);

#define SWITCH_ADDRESS_REGION(address, r0, user, blip, r1, pmm1, r2, sys) \
	if(address >= PMM0_R0_BASE && address < PMM0_R0_LIMIT) {\
		r0\
	} else if(address >= PMM0_USER_BASE && address < PMM0_USER_LIMIT) {\
		user\
	} else if(address >= PMM0_BLIP_BASE && address < PMM0_BLIP_LIMIT) {\
		blip\
	} else if(address >= PMM0_R1_BASE && address < PMM0_R1_LIMIT) {\
		r1\
	} else if(address >= PMM0_PMM1_BASE && address < PMM0_PMM1_LIMIT) {\
		pmm1\
	} else if(address >= PMM0_R2_BASE && address < PMM0_R2_LIMIT) {\
		r2\
	} else {\
		sys\
	}

#define SWITCH_MEM_IO(mem, addr, rw, size_bits, io) \
	switch(size_bits) {\
		case 8: {\
			if(!rw) *((byte*) io) = mem_read8(mem, addr);\
			else mem_write8(mem, addr, *((byte*) io));\
			break;\
		}\
		case 16: {\
			if(!rw) *((half_word*) io) = mem_read16(mem, addr);\
			else mem_write16(mem, addr, *((half_word*) io));\
			break;\
		}\
		case 32: {\
			if(!rw) *((word*) io) = mem_read32(mem, addr);\
			else mem_write32(mem, addr, *((word*) io));\
			break;\
		}\
		default:\
			assert(false);\
	}

bool _mmu_hw_address_io(p_addr address, bool rw, u8 size_bits, void* io) {
	assert(io);

	word cpl = cu_read_reg(REG_RPL);
	
	if(address >= PMM0_R0_BASE && address < PMM0_R0_LIMIT) {
		cu_raise_int(INT_MAE, address, INT_MAE_1_INVALID_ADDRESS);
		return false;
	} else if(address >= PMM0_USER_BASE && address < PMM0_USER_LIMIT) {
		for(usize i = 0; i < PMM1_ENTRY_COUNT; i++) {
			PMM1_Entry* entry = (PMM1_Entry*) l1_pmm.ptr + (i * sizeof(PMM1_Entry));

			// base and limit in entry are 0x1000-aligned and shifted right; need to "restore" them
			if(entry->tg != 0 && address >= (entry->bs << 12) && address < (entry->lm << 12)) {
				p_addr address_tg = address - (entry->bs << 12);
				u8 idx = entry->idx;

				switch(entry->tg) {
					case FT_PIT: {
						if(idx >= PIT_COUNT) {
							cu_raise_int(INT_IOP, address, INT_IOP_0_OUT_OF_RANGE);
							return false;
						}

						if(size_bits != sizeof(word) * 8) {
							cu_raise_int(INT_IOP, address, INT_IOP_0_WRONG_DATA_SIZE);
							return false;
						}

						PIT* pit = &pit_timers[idx];

						if(!rw) *((word*) io) = pit_io_read(pit, address_tg);
						else pit_io_write(pit, address_tg, *((word*) io));

						return true;
					}
					case FT_UART: {
						if(idx >= UART_COUNT) {
							cu_raise_int(INT_IOP, address, INT_IOP_0_OUT_OF_RANGE);
							return false;
						}

						if(size_bits != sizeof(word) * 8) {
							cu_raise_int(INT_IOP, address, INT_IOP_0_WRONG_DATA_SIZE);
							return false;
						}

						Uart* u = &uart[0];

						if(!rw) *((word*) io) = uart_io_read(u, address_tg);
						else uart_io_write(u, address_tg, *((word*) io));

						return true;
					}
					default:
						cu_raise_int(INT_IOP, INT_IOP_0_UNKNOWN_FEATURE, 0);
						return false;
				}
			}

			SWITCH_MEM_IO(&user_ram, address - PMM0_USER_BASE, rw, size_bits, io);
			return true;
		}
	} else if(address >= PMM0_BLIP_BASE && address < PMM0_BLIP_LIMIT) {
		if(cpl != PL_1) {
			cu_raise_int(INT_GPE, INT_GPE_0_WRONG_PRIVILEGE, INT_GPE_1(cpl, PL_1));
			return false;
		}

		SWITCH_MEM_IO(&blip_rom, address - PMM0_BLIP_BASE, rw, size_bits, io);
		return true;
	} else if(address >= PMM0_R1_BASE && address < PMM0_R1_LIMIT) {
		cu_raise_int(INT_MAE, address, INT_MAE_1_INVALID_ADDRESS);
		return false;
	} else if(address >= PMM0_PMM1_BASE && address < PMM0_PMM1_LIMIT) {
		if(cpl != PL_1) {
			cu_raise_int(INT_GPE, INT_GPE_0_WRONG_PRIVILEGE, INT_GPE_1(cpl, PL_1));
			return false;
		}

		SWITCH_MEM_IO(&l1_pmm, address - PMM0_PMM1_BASE, rw, size_bits, io);
		return true;
	} else if(address >= PMM0_R2_BASE && address < PMM0_R2_LIMIT) {
		cu_raise_int(INT_MAE, address, INT_MAE_1_INVALID_ADDRESS);
		return false;
	} else {
		if(cpl != PL_1) {
			cu_raise_int(INT_GPE, INT_GPE_0_WRONG_PRIVILEGE, INT_GPE_1(cpl, PL_1));
			return false;
		}

		SWITCH_MEM_IO(&system_ram, address - PMM0_SYSTEM_BASE, rw, size_bits, io);
		return true;
	}

	assert(false);
	return false;
}

// the level-1 PMM is meant to be stored in the CPU itself so this is fine
HardwareMemory l1_pmm = {0};

p_addr mmu_tavp(v_addr address, char purpose) {
	assert(purpose == 'r' || purpose == 'w' || purpose == 'x');

	word rpdp = cu_read_reg(REG_RPDP);

	// 4.6.7: rpdp == 0 -> paging disabled
	if(rpdp == 0) {
		return address;
	}

	usize PD_SIZE = sizeof(PageDirectory) / sizeof(PageDirectoryEntry);
	usize PT_SIZE = sizeof(PageTable) / sizeof(PageDescriptor);

	u8 page_offset = address & 0xFFF;
	u8 l0_pd_index = address >> 24;
	u8 l1_pd_index = (address >> 16) & (PD_SIZE - 1);
	u8 pt_index = (address >> 12) & (PT_SIZE - 1);

	assert(l0_pd_index < PD_SIZE);
	assert(l1_pd_index < PD_SIZE);
	assert(pt_index < PT_SIZE);

	PageDirectory* l0_pd = g2h_phy_addr(rpdp);
	assert(l0_pd);

	PageDirectoryEntry* l0_pd_entry = &l0_pd->entries[l0_pd_index];

	if(!l0_pd_entry->p /*|| l0_pd_entry.adr == 0*/) {
		cu_raise_int(INT_PLE, address, INT_PLE_1_NOT_FOUND);
		return 0;
	}

	PageDirectory* l1_pd = g2h_phy_addr(l0_pd_entry->adr);
	assert(l1_pd);

	PageDirectoryEntry* l1_pd_entry = &l1_pd->entries[l1_pd_index];

	if(!l1_pd_entry->p /*|| l1_pd_entry.adr == 0*/) {
		cu_raise_int(INT_PLE, address, INT_PLE_1_NOT_FOUND);
		return 0;
	}

	PageTable* pt = g2h_phy_addr(l1_pd_entry->adr);
	assert(pt);

	PageDescriptor* pdesc = &pt->entries[pt_index];

	if(!pdesc->p) {
		cu_raise_int(INT_PLE, address, INT_PLE_1_NOT_FOUND);
		return 0;
	}

	word cpl = cu_read_reg(REG_RPL);

	if(cpl != pdesc->pl) {
		cu_raise_int(INT_GPE, INT_GPE_0_WRONG_PRIVILEGE, INT_GPE_1(cpl, pdesc->pl));
		return 0;
	}

	if(purpose == 'w' && !pdesc->rw) {
		cu_raise_int(INT_GPE, INT_GPE_0_WRITE_ON_READ_ONLY, INT_GPE_1(cpl, pdesc->pl));
		return 0;
	}

	if(purpose == 'x' && !pdesc->ex) {
		cu_raise_int(INT_GPE, INT_GPE_0_EXEC_ON_NO_EXEC, INT_GPE_1(cpl, pdesc->pl));
		return 0;
	}

	return pdesc->adr + page_offset;
}

byte mmu_mread8(p_addr address) {
	byte out = 0;
	_mmu_hw_address_io(address, false, 8, &out);
	return out;
}

half_word mmu_mread16(p_addr address) {
	if((address & 0b1) != 0) {
		cu_raise_int(INT_MAE, address, INT_MAE_1_WRONG_ALIGNMENT);
		return 0;
	}

	half_word out = 0;
	_mmu_hw_address_io(address, false, 16, &out);
	return out;
}

word mmu_mread32(p_addr address) {
	if((address & 0b11) != 0) {
		cu_raise_int(INT_MAE, address, INT_MAE_1_WRONG_ALIGNMENT);
		return 0;
	}

	word out = 0;
	_mmu_hw_address_io(address, false, 32, &out);
	return out;
}

bool mmu_mwrite8(p_addr address, byte data) {
	return _mmu_hw_address_io(address, true, 8, &data);
}

bool mmu_mwrite16(p_addr address, half_word data) {
	if((address & 0b1) != 0) {
		cu_raise_int(INT_MAE, address, INT_MAE_1_WRONG_ALIGNMENT);
		return false;
	}

	return _mmu_hw_address_io(address, true, 16, &data);
}

bool mmu_mwrite32(p_addr address, word data) {
	if((address & 0b11) != 0) {
		cu_raise_int(INT_MAE, address, INT_MAE_1_WRONG_ALIGNMENT);
		return false;
	}

	return _mmu_hw_address_io(address, true, 32, &data);
}

void* g2h_phy_addr(p_addr address) {
	SWITCH_ADDRESS_REGION(
		address,
		{ return NULL; },
		{
			p_addr rel_addr = address - PMM0_USER_BASE;
			if(rel_addr >= user_ram.size) return 0; // TODO #MAE (in MMU) on memory access out of physical limits?
			return user_ram.ptr + rel_addr;
		},
		{
			p_addr rel_addr = address - PMM0_BLIP_BASE;
			if(rel_addr >= blip_rom.size) return 0;
			return blip_rom.ptr + rel_addr;
		},
		{ return NULL; },
		{ return l1_pmm.ptr + address - PMM0_PMM1_BASE; },
		{ return NULL; },
		{
			p_addr rel_addr = address - PMM0_SYSTEM_BASE;
			if(rel_addr >= system_ram.size) return 0;
			return system_ram.ptr + rel_addr;
		}
	);
}

p_addr h2g_phy_addr(void* address) {
	#define H2G_BRANCH(base, ptr, size) \
		if(address >= (void*) ptr && address < ((void*) ptr + size)) { \
			return base + ((usize) address - (usize) ptr); \
		}

	H2G_BRANCH(PMM0_USER_BASE, user_ram.ptr, user_ram.size);
	H2G_BRANCH(PMM0_BLIP_BASE, blip_rom.ptr, blip_rom.size);
	H2G_BRANCH(PMM0_PMM1_BASE, l1_pmm.ptr, l1_pmm.size);
	H2G_BRANCH(PMM0_SYSTEM_BASE, system_ram.ptr, system_ram.size);

	return 0;
}

void g2h_read32(p_addr address, void* out, word size) {
	for(word i = 0; i < size / sizeof(word); i++) {
		word data = mmu_mread32(address + i * sizeof(word));
		memmove(out + i * sizeof(word), &data, sizeof(word));
	}
}

void h2g_write32(void* src, p_addr out, word size) {
	for(word i = 0; i < size / sizeof(word); i++) {
		word data = 0;
		memcpy(&data, src + i * sizeof(word), sizeof(word));
		mmu_mwrite32(out + i * sizeof(word), data);
	}
}
