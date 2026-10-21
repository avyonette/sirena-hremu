#include "dbg.h"
#include "cpu/cu.h"
#include "cpu/mmu.h"
#include "cpu/reg.h"
#include <assert.h>

#define FMT "%08X"

void dbg_cpu_dump_regs(FILE* out) {
	fprintf(out, "/- LOCAL --------+----------------+----------------+----------------\\\n");

	// r0-r15
	fprintf(out, "| r0  : "FMT" | r1  : "FMT" | r2  : "FMT" | r3  : "FMT" |\n", cu_read_reg(REG_R0), cu_read_reg(REG_R1), cu_read_reg(REG_R2), cu_read_reg(REG_R3));
	fprintf(out, "| r4  : "FMT" | r5  : "FMT" | r6  : "FMT" | r7  : "FMT" |\n", cu_read_reg(REG_R4), cu_read_reg(REG_R5), cu_read_reg(REG_R6), cu_read_reg(REG_R7));
	fprintf(out, "| r8  : "FMT" | r9  : "FMT" | r10 : "FMT" | r11 : "FMT" |\n", cu_read_reg(REG_R8), cu_read_reg(REG_R9), cu_read_reg(REG_R10), cu_read_reg(REG_R11));
	fprintf(out, "| r12 : "FMT" | r13 : "FMT" | r14 : "FMT" | r15 : "FMT" |\n", cu_read_reg(REG_R12), cu_read_reg(REG_R13), cu_read_reg(REG_R14), cu_read_reg(REG_R15));

	fprintf(out, "|----------------|----------------|----------------|----------------|\n");

	// rip-rpl
	fprintf(out, "| rip : "FMT" | rtp : "FMT" | rsp : "FMT" | rlfp: "FMT" |\n", cu_read_reg(REG_RIP), cu_read_reg(REG_RTP), cu_read_reg(REG_RSP), cu_read_reg(REG_RLFP));
	fprintf(out, "| rpdp: "FMT" | rcf : "FMT" | rpl : "FMT" |                |\n", cu_read_reg(REG_RPDP), cu_read_reg(REG_RCF), cu_read_reg(REG_RPL));

	fprintf(out, "|----------------|----------------|----------------|----------------|\n");

	// rfi, rfo
	fprintf(out, "| rfi0: "FMT" | rfi1: "FMT" | rfo0: "FMT" | rfo1: "FMT" |\n", cu_read_reg(REG_RFI0), cu_read_reg(REG_RFI1), cu_read_reg(REG_RFO0), cu_read_reg(REG_RFO1));

	fprintf(out, "|- GLOBAL -------|----------------|----------------|----------------|\n");

	// rfp, rrap, rz
	fprintf(out, "| rfp : "FMT" | rrap: "FMT" | rz  : "FMT" |                |\n", cu_read_reg(REG_RFP), cu_read_reg(REG_RRAP), cu_read_reg(REG_RZ));

	fprintf(out, "\\----------------+----------------+----------------+----------------/\n");
}

void dbg_cpu_get_rip_data32(word* data) {
	assert(data);

	word rip = cu_read_reg(REG_RIP);
	assert(rip > 0);

	word* data_ptr = g2h_phy_addr(mmu_tavp(rip, 'r'));
	assert(data_ptr);
	
	*data = *data_ptr;
}

void dbg_cpu_read32_v(v_addr addr, word* data) {
	assert(data);

	word* data_ptr = g2h_phy_addr(mmu_tavp(addr, 'r'));
	
	if(data_ptr == NULL) {
		*data = 0;
		return;
	}
	
	*data = *data_ptr;
}
