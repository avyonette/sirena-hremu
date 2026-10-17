#include <string.h>
#include "log.h"
#include "reg.h"
#include "cu.h"
#include "mmu.h"
#include "isa/isa.h"

bool irq_map[INT_MAX + 1] = { 0 };

word rfp = 0;
word rrap = 0;
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
	rfp = PMM0_SYSTEM_BASE;
	rrap = 0x0;
}

void cu_step() {
fetch: ;
	word inst_data = mmu_mread32(mmu_tavp(cu_read_reg(REG_RIP), 'x'));

decode: ;
	CpuInstruction inst;
	isa_decode(&inst, inst_data);

	zlog_info(lc_emu, "decoded instr op %d", inst.op);

execute: ;
	isa_execute(&inst);

interrupts: ;
	// handle interrupts
	/*
		TODO
		spec says that "If the rrap register is set to 0, all interrupts [...], are ignored", but specifying what "ignored" means is necessary; does it clear the irq (done here) or does it not?
		"If the enable bit of the IRA entry is not set, the interrupt stops being handed over to be handled in software" is more explicit about its meaning, but wording is also weird
	*/
	for(u8 i = 0; i < INT_MAX; i++) {
		if(irq_map[i]) {
			if(cu_read_reg(rrap)) {
				InterruptRoutineArrayEntry irae;
				g2h_read32(rrap + (i * sizeof(InterruptRoutineArrayEntry)), &irae, sizeof(InterruptRoutineArrayEntry));
				
				if(irae.en) {
					cu_write_reg(REG_RFP, irae.rfp);
					cu_write_reg(REG_RIP, irae.rh);

					zlog_info(lc_cpu, "handling INT %#02x (rfp=%#10x, rh=%#10x)", i, irae.rfp, irae.rh);

					goto fetch; // need to handle this interrupt now
				}
			}
			
			irq_map[i] = 0;
		}
	}

advance: ;
	if(inst.advance_next) {
		cu_write_reg(REG_RIP, cu_read_reg(REG_RIP) + sizeof(word));
	}
}

void cu_raise_int(u8 id, word rfi0, word rfi1) {
	irq_map[id] = true;
	zlog_info(lc_cpu, "INT %#04x raised (rfi0=%#10x, rfi1=%#10x)", id, rfi0, rfi1);
	
	if(rrap) {
		InterruptRoutineArrayEntry irae;
		g2h_read32(rrap + (id * sizeof(InterruptRoutineArrayEntry)), &irae, sizeof(InterruptRoutineArrayEntry));

		mmu_mwrite32(irae.rfp + offsetof(RegisterFrame, rfi0), rfi0);
		mmu_mwrite32(irae.rfp + offsetof(RegisterFrame, rfi1), rfi1);
	}
}

word cu_read_reg(u8 idx) {
	switch(idx) {
		case REG_RFP: {
			return rfp;
		}
		case REG_RRAP: {
			return rrap;
		}
		case REG_RZ: {
			return 0;
		}
		default: {
			assert(rfp);
			return mmu_mread32(rfp + (idx * sizeof(word)));
		}
	}
}

bool cu_write_reg(u8 idx, word value) {
	switch(idx) {
		case REG_RFP: {
			PL_CHECK_RET(PL_1, false)
			rfp = value;
		}
		case REG_RRAP: {
			PL_CHECK_RET(PL_1, false)
			rrap = value;
		}
		case REG_RZ: {
			cu_raise_int(INT_IOP, 0, 0);
			return false;
		}
		case REG_RPDP:
			PL_CHECK_RET(PL_1, false)
		default: {
			assert(rfp);
			mmu_mwrite32(rfp + (idx * sizeof(word)), value);
		}
	}

	return true;
}
