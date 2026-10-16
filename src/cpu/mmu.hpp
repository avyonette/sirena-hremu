#pragma once
#include "mem/mem.hpp"
#include <types.h>
#include <arch_types.h>

#define PMM0_R0_BASE 0x00000000
#define PMM0_R0_LIMIT 0x00001000

#define PMM0_USER_BASE 0x00001000
#define PMM0_USER_LIMIT 0xFFA00000

#define PMM0_BLIP_BASE 0xFFA00000
#define PMM0_BLIP_LIMIT 0xFFA10000

#define PMM0_R1_BASE 0xFFA10000
#define PMM0_R1_LIMIT 0xFFFDFC00

#define PMM0_SPPBC_BASE 0xFFFDFC00
#define PMM0_SPPBC_LIMIT 0xFFFDFE00

#define PMM0_PMM1_BASE 0xFFFDFE00
#define PMM0_PMM1_LIMIT 0xFFFE0000

#define PMM0_SYSTEM_BASE 0xFFFE0000
#define PMM0_SYSTEM_LIMIT 0xFFFFFFFF

namespace hremu::cpu::mmu {
	extern mem::ReadWriteMemory l1_pmm;
	
	p_addr tavp(v_addr address, char purpose);

	byte mread8(p_addr address);
	half_word mread16(p_addr address);
	word mread32(p_addr address);

	void mwrite8(p_addr address, byte data);
	void mwrite16(p_addr address, half_word data);
	void mwrite32(p_addr address, word data);

	/*
		converts a guest emulator's physical address to a "real" host address
		used for easily reading structures that are located in the guest's fictional memory
	*/
	void* g2h_phy_addr(p_addr address);
	// does the same as the above but in reverse
	p_addr h2g_phy_addr(void* address);
}
