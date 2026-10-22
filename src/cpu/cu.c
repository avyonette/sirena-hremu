#include <string.h>
#include "cpu/clk.h"
#include "cpu/int.h"
#include "emu.h"
#include "hw/pit.h"
#include "log.h"
#include "mem/mem.h"
#include "mem/ram.h"
#include "reg.h"
#include "cu.h"
#include "mmu.h"
#include "isa/isa.h"

bool irq_map[INT_MAX + 1] = { 0 };

word rfp = 0;
word rrap = 0;
word rz = 0;

usize cu_clk_0 = 0;
usize cu_clk_1 = 1;
usize cu_clk_2 = 2;
usize cu_clk_3 = 3;
usize cu_clk_4 = 4;

void _cu_write_reg_direct(u8 idx, word value);

void cu_init() {
	// init clocks
	//clk_create(cu_clk_0, CLK_0_FREQ, );
	clk_create(cu_clk_1, CLK_1_FREQ, pit_tick_0, false);
	clk_create(cu_clk_2, CLK_2_FREQ, pit_tick_1, false);
	clk_create(cu_clk_3, CLK_3_FREQ, pit_tick_2, false);
	clk_create(cu_clk_4, CLK_4_FREQ, pit_tick_3, false);
}

void cu_reset() {
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
	memcpy(g2h_phy_addr(0xFFFFFF00), &rf, sizeof(RegisterFrame));

	// set default global register values
	rfp = 0xFFFFFF00;
	rrap = 0x0;

	// initialise level-1 PMM
	mem_initc(&l1_pmm, PMM1_ENTRY_COUNT * sizeof(PMM1_Entry), true);

	// enable clocks
	clk_resume(cu_clk_0);
	clk_resume(cu_clk_1);
	clk_resume(cu_clk_2);
	clk_resume(cu_clk_3);
	clk_resume(cu_clk_4);
}

void cu_step() {
fetch: ;
	word inst_data = mmu_mread32(mmu_tavp(cu_read_reg(REG_RIP), 'x'));
	//EMU_INFO("inst data %#010x", inst_data);

decode: ;
	CpuInstruction inst;

	if(!isa_decode(&inst, inst_data)) {
		cu_raise_int(INT_UDO, INT_UDO_0(inst.op, inst.os), 0);
		goto interrupts; // skip execute
	}

execute: ;
	//EMU_INFO("exec op %#04x", inst.op);

	if(!isa_execute(&inst)) {
		cu_raise_int(INT_UDO, INT_UDO_0(inst.op, inst.os), 0);
		goto interrupts;
	}

interrupts: ;
	// handle interrupts
	/*
		TODO
		spec says that "If the rrap register is set to 0, all interrupts [...], are ignored", but specifying what "ignored" means is necessary; does it clear the irq (done here) or does it not?
		"If the enable bit of the IRA entry is not set, the interrupt stops being handed over to be handled in software" is more explicit about its meaning, but wording is also weird
	*/
	for(u8 i = 0; i < INT_MAX; i++) {
		if(irq_map[i]) {
			if(rrap > 0) {
				InterruptRoutineArrayEntry irae;
				g2h_read32(rrap + (i * sizeof(InterruptRoutineArrayEntry)), &irae, sizeof(InterruptRoutineArrayEntry));
				
				if(irae.en) {
					word prev_rfp = cu_read_reg(REG_RFP);

					_cu_write_reg_direct(REG_RFP, irae.rfp);
					_cu_write_reg_direct(REG_RPL, 1);
					_cu_write_reg_direct(REG_RIP, irae.rh);
					_cu_write_reg_direct(REG_RLFP, prev_rfp);

					CPU_INFO("handling INT %#02x (rfp=%#10x, rh=%#10x)", i, irae.rfp, irae.rh);

					irq_map[i] = false;
					goto fetch; // need to handle this interrupt now
				}
			}
		}
	}

advance: ;
	// 7. "the CPU must advance the (rip) by a single word for any instruction that does not explicitly set the (rip)"
	if(inst.auto_advance) {
		cu_write_reg(REG_RIP, cu_read_reg(REG_RIP) + sizeof(word));
	}
}

void cu_pause() {
	clk_pause(cu_clk_0);
	clk_pause(cu_clk_1);
	clk_pause(cu_clk_2);
	clk_pause(cu_clk_3);
	clk_pause(cu_clk_4);
}

void cu_resume() {
	clk_resume(cu_clk_0);
	clk_resume(cu_clk_1);
	clk_resume(cu_clk_2);
	clk_resume(cu_clk_3);
	clk_resume(cu_clk_4);
}

void cu_raise_int(u8 id, word rfi0, word rfi1) {
	irq_map[id] = true;
	CPU_INFO("INT %#04x raised (rfi0=%#10x, rfi1=%#10x)", id, rfi0, rfi1);
	
	if(id == INT_DBG) { // pause execution on #DBG
		cu_pause();
		emu_run = false;
	}

	if(rrap > 0) {
		InterruptRoutineArrayEntry irae;
		g2h_read32(rrap + (id * sizeof(InterruptRoutineArrayEntry)), &irae, sizeof(InterruptRoutineArrayEntry));

		if(!irae.en) return;

		mmu_mwrite32(irae.rfp + offsetof(RegisterFrame, rfi0), rfi0);
		mmu_mwrite32(irae.rfp + offsetof(RegisterFrame, rfi1), rfi1);
	}
}

void cu_enable_irh(u8 id, bool enable) {
	if(!rrap) return;

	InterruptRoutineArrayEntry irae;
	g2h_read32(rrap + (id * sizeof(InterruptRoutineArrayEntry)), &irae, sizeof(InterruptRoutineArrayEntry));

	irae.en = enable;
	h2g_write32(&irae, rrap + (id * sizeof(InterruptRoutineArrayEntry)), sizeof(InterruptRoutineArrayEntry));
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
			return mem_read32(&system_ram, (rfp - PMM0_SYSTEM_BASE) + (idx * sizeof(word)));
		}
	}
}

void cu_write_reg(u8 idx, word value) {
	switch(idx) {
		case REG_RFP: {
			PL_CHECK(PL_1)

			// check if new fp is inside system RAM
			if(value >= PMM0_SYSTEM_BASE && value < PMM0_SYSTEM_LIMIT) {
				rfp = value;
			} else {
				cu_raise_int(INT_IOP, INT_IOP_0_FP_OUTSIDE_SYSTEM, 0);
			}
			break;
		}
		case REG_RRAP: {
			PL_CHECK(PL_1)
			rrap = value;
			break;
		}
		case REG_RLFP:
		case REG_RZ: {
			cu_raise_int(INT_IOP, INT_IOP_0_WRITE_TO_RO_REG, 0);
			break;
		}
		case REG_RPL:
			if(value != PL_0 && value != PL_1) {
				cu_raise_int(INT_GPE, INT_GPE_0_UNDEFINED_PRIVILEGE, 0);
				break; // requested PL is invalid
			}
		case REG_RPDP:
			PL_CHECK(PL_1)
		default: {
			assert(rfp);
			mem_write32(&system_ram, (rfp - PMM0_SYSTEM_BASE) + (idx * sizeof(word)), value);
			break;
		}
	}
}

void _cu_write_reg_direct(u8 idx, word value) {
	switch(idx) {
		case REG_RFP: {
			rfp = value;
			break;
		}
		case REG_RRAP: {
			rrap = value;
			break;
		}
		default: {
			assert(rfp);
			mem_write32(&system_ram, (rfp - PMM0_SYSTEM_BASE) + (idx * sizeof(word)), value);
			break;
		}
	}
}
