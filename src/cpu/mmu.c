#include "mmu.h"
#include "cu.h"
#include "paging.h"
#include "mem/ram.h"
#include "mem/rom.h"
#include <stdlib.h>

#define READ_REGION(address, size) \
	if(address >= PMM0_R0_BASE && address < PMM0_R1_LIMIT) { \
		cu_raise_int(INT_MAE, address, INT_MAE_1_INVALID_ADDRESS); \
		return 0; \
	} else if(address >= PMM0_USER_BASE && address < PMM0_USER_LIMIT) { \
		return mem_read##size(&user_ram, address - PMM0_USER_BASE); \
	} else if(address >= PMM0_BLIP_BASE && address < PMM0_BLIP_LIMIT) { \
		return mem_read##size(&blip_rom, address - PMM0_BLIP_BASE); \
	} else if(address >= PMM0_R1_BASE && address < PMM0_R1_LIMIT) { \
		cu_raise_int(INT_MAE, address, INT_MAE_1_INVALID_ADDRESS); \
		return 0; \
	} else if(address >= PMM0_SPPBC_BASE && address < PMM0_SPPBC_LIMIT) { \
		assert(false && "not implemented"); /* TODO SPPBC map */ \
		return 0; \
	} else if(address >= PMM0_PMM1_BASE && address < PMM0_PMM1_LIMIT) { \
		return mem_read##size(&l1_pmm, address - PMM0_PMM1_BASE); \
	} else { \
		return mem_read##size(&system_ram, address - PMM0_SYSTEM_BASE); \
	}

#define WRITE_REGION(address, size, data) \
	if(address >= PMM0_R0_BASE && address < PMM0_R1_LIMIT) { \
		cu_raise_int(INT_MAE, address, INT_MAE_1_INVALID_ADDRESS); \
	} else if(address >= PMM0_USER_BASE && address < PMM0_USER_LIMIT) { \
		mem_write##size(&user_ram, address - PMM0_USER_BASE, data); \
	} else if(address >= PMM0_BLIP_BASE && address < PMM0_BLIP_LIMIT) { \
		cu_raise_int(INT_MAE, address, INT_MAE_1_WRITE_TO_ROM); \
	} else if(address >= PMM0_R1_BASE && address < PMM0_R1_LIMIT) { \
		cu_raise_int(INT_MAE, address, INT_MAE_1_INVALID_ADDRESS); \
	} else if(address >= PMM0_SPPBC_BASE && address < PMM0_SPPBC_LIMIT) { \
		assert(false && "not implemented"); /* TODO SPPBC map */ \
	} else if(address >= PMM0_PMM1_BASE && address < PMM0_PMM1_LIMIT) { \
		mem_write##size(&l1_pmm, address - PMM0_PMM1_BASE, data); \
	} else { \
		mem_write##size(&system_ram, address - PMM0_SYSTEM_BASE, data); \
	}

// the level-1 PMM is meant to be stored in the CPU itself so this is fine
HardwareMemory l1_pmm = {0};

p_addr mmu_tavp(v_addr address, char purpose) {
	assert(purpose == 'r' || purpose == 'w' || purpose == 'x');

	// 4.6.7: rpdp == 0 -> paging disabled
	if(rfp->rpdp == 0) {
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

	PageDirectory* l0_pd = g2h_phy_addr(rfp->rpdp);
	assert(pdp);

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

	if(rfp->rpl != pdesc->pl) {
		cu_raise_int(INT_GPE, INT_GPE_0_WRONG_PRIVILEGE, INT_GPE_1(rfp->rpl, pdesc->pl));
		return 0;
	}

	if(purpose == 'w' && !pdesc->rw) {
		cu_raise_int(INT_GPE, INT_GPE_0_WRITE_ON_READ_ONLY, INT_GPE_1(rfp->rpl, pdesc->pl));
		return 0;
	}

	if(purpose == 'x' && !pdesc->ex) {
		cu_raise_int(INT_GPE, INT_GPE_0_EXEC_ON_NO_EXEC, INT_GPE_1(rfp->rpl, pdesc->pl));
		return 0;
	}

	return pdesc->adr + page_offset;
}

byte mmu_mread8(p_addr address) {
	READ_REGION(address, 8);
}

half_word mmu_mread16(p_addr address) {
	if((address & 0xF) != 0) {
		cu_raise_int(INT_MAE, address, INT_MAE_1_WRONG_ALIGNMENT);
		return 0;
	}

	READ_REGION(address, 16);
}

word mmu_mread32(p_addr address) {
	if((address & 0x1F) != 0) {
		cu_raise_int(INT_MAE, address, INT_MAE_1_WRONG_ALIGNMENT);
		return 0;
	}

	READ_REGION(address, 32);
}

void mmu_mwrite8(p_addr address, byte data) {
	WRITE_REGION(address, 8, data);
}

void mmu_mwrite16(p_addr address, half_word data) {
	if((address & 0xF) != 0) {
		cu_raise_int(INT_MAE, address, INT_MAE_1_WRONG_ALIGNMENT);
		return;
	}

	WRITE_REGION(address, 16, data);
}

void mmu_mwrite32(p_addr address, word data) {
	if((address & 0x1F) != 0) {
		cu_raise_int(INT_MAE, address, INT_MAE_1_WRONG_ALIGNMENT);
		return;
	}

	WRITE_REGION(address, 32, data);
}

void* g2h_phy_addr(p_addr address) {
	if(address >= PMM0_R0_BASE && address < PMM0_R0_LIMIT) {
		return 0;
	} else if(address >= PMM0_USER_BASE && address < PMM0_USER_LIMIT) {
		return user_ram.ptr + address - PMM0_USER_BASE;
	} else if(address >= PMM0_BLIP_BASE && address < PMM0_BLIP_LIMIT) {
		return blip_rom.ptr + address - PMM0_BLIP_BASE;
	} else if(address >= PMM0_R1_BASE && address < PMM0_R1_LIMIT) {
		return 0;
	} else if(address >= PMM0_SPPBC_BASE && address < PMM0_SPPBC_LIMIT) {
		assert(false && "not implemented"); /* TODO SPPBC map */
		return 0;
	} else if(address >= PMM0_PMM1_BASE && address < PMM0_PMM1_LIMIT) {
		return l1_pmm.ptr + address - PMM0_PMM1_BASE;
	} else {
		return system_ram.ptr + address - PMM0_SYSTEM_BASE;
	}
}

p_addr h2g_phy_addr(void* address) {
	#define H2G_BRANCH(base, ptr, size) \
		if(address >= (void*) ptr && address < ((void*) ptr + size)) { \
			return base + ((usize) address - (usize) ptr); \
		}

	H2G_BRANCH(PMM0_USER_BASE, user_ram.ptr, user_ram.size);
	H2G_BRANCH(PMM0_BLIP_BASE, blip_rom.ptr, blip_rom.size);
	// TODO SPPBC
	H2G_BRANCH(PMM0_PMM1_BASE, l1_pmm.ptr, l1_pmm.size);
	H2G_BRANCH(PMM0_SYSTEM_BASE, system_ram.ptr, system_ram.size);

	assert(false);
	return 0;
}
