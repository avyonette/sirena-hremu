#include <string.h>
#include "cu.h"
#include "mmu.h"

bool irq_map[INT_MAX + 1] = { 0 };

RegisterFrame* rfp = 0;
InterruptRoutineArray* rrap = 0;
word rz = 0;

void cu_init() {
	// create default register frame
	RegisterFrame rf = {
		.r0 = 0, .r1 = 0, .r2 = 0, .r3 = 0, .r4 = 0, .r5 = 0, .r6 = 0, .r7 = 0,
		.r8 = 0, .r9 = 0, .r10 = 0, .r11 = 0, .r12 = 0, .r13 = 0, .r14 = 0, .r15 = 0,
		.rip = 0xFFA00000,
		.rtp = 0x0,
		.rsp = 0x0,
		.rpdp = 0x0,
		.rcf = 0,
		.rpl = 1,
		.rfi0 = 0, .rfi1 = 0,
		.rfo0 = 0, .rfo1 = 0
	};

	// write default register frame to the beginning of system RAM
	memcpy(g2h_phy_addr(PMM0_SYSTEM_BASE), &rf, sizeof(RegisterFrame));

	// set default global register values
	rfp = g2h_phy_addr(PMM0_SYSTEM_BASE);
	rrap = 0x0;
}

void step() {
fetch: ;
	word instruction = mmu_mread32(mmu_tavp(rfp->rip, 'x'));

decode: ;
	// decode TODO

execute: ;
	// execute TODO

interrupts: ;
	// handle interrupts
	/*
		TODO
		spec says that "If the rrap register is set to 0, all interrupts [...], are ignored", but specifying what "ignored" means is necessary; does it clear the irq (done here) or does it not?
		"If the enable bit of the IRA entry is not set, the interrupt stops being handed over to be handled in software" is more explicit about its meaning, but wording is also weird
	*/
	for(u8 i = 0; i < INT_MAX; i++) {
		if(irq_map[i]) {
			if(rrap) {
				InterruptRoutineArrayEntry* irae = &rrap->entries[i];
				
				if(irae->en) {
					rfp = g2h_phy_addr(irae->rfp);
					rfp->rip = irae->rh;
					goto fetch; // need to handle this interrupt now
				}
			}
			
			irq_map[i] = 0;
		}
	}

advance: ;
	rfp->rip += sizeof(word);
}

void raise_int(u8 id, word rfi0, word rfi1) {
	irq_map[id] = true;
	
	if(rrap) {
		InterruptRoutineArrayEntry* irae = &rrap->entries[id];

		mmu_mwrite32(irae->rfp + offsetof(RegisterFrame, rfi0), rfi0);
		mmu_mwrite32(irae->rfp + offsetof(RegisterFrame, rfi1), rfi1);
	}
}
