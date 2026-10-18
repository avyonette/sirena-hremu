#include "core32.h"
#include "cpu/alu.h"
#include "cpu/cu.h"
#include "cpu/int.h"
#include "cpu/mmu.h"
#include "isa/op.h"

bool nop(CpuInstruction* inst) {
	return true;
}

bool cpuid(CpuInstruction* inst) {
	cu_write_reg(inst->r.ds, 1); // CORE32 set
	return true;
}

bool ldw(CpuInstruction* inst) {
	switch(inst->r.ff) {
		case CORE32_LDW_LDW: {
			word source = cu_read_reg(inst->r.ra);
			word data = mmu_mread32(mmu_tavp(source, 'r'));
			cu_write_reg(inst->r.ds, data);
			return true;
		}
		case CORE32_LDW_LDHZ: {
			word source = cu_read_reg(inst->r.ra);
			word data = mmu_mread16(mmu_tavp(source, 'r'));
			cu_write_reg(inst->r.ds, alu_zeroe16(data));
			return true;
		}
		case CORE32_LDW_LDHS: {
			word source = cu_read_reg(inst->r.ra);
			word data = mmu_mread16(mmu_tavp(source, 'r'));
			cu_write_reg(inst->r.ds, alu_signe16(data));
			return true;
		}
		case CORE32_LDW_LDBZ: {
			word source = cu_read_reg(inst->r.ra);
			word data = mmu_mread8(mmu_tavp(source, 'r'));
			cu_write_reg(inst->r.ds, alu_zeroe8(data));
			return true;
		}
		case CORE32_LDW_LDBS: {
			word source = cu_read_reg(inst->r.ra);
			word data = mmu_mread8(mmu_tavp(source, 'r'));
			cu_write_reg(inst->r.ds, alu_signe8(data));
			return true;
		}
	}

	return false;
}

bool lvi(CpuInstruction* inst) {
	cu_write_reg(inst->i.ds, alu_signe16(inst->i.vl));
	return true;
}

bool lli(CpuInstruction* inst) {
	switch(inst->i.ff) {
		case CORE32_LLI_LLI: {
			word value = inst->i.vl;
			word destination = cu_read_reg(inst->i.ds) & 0xFFFF0000;

			cu_write_reg(inst->i.ds, destination | value);
			return true;
		}
		case CORE32_LLI_LUI: {
			word value = inst->i.vl << 16;
			word destination = cu_read_reg(inst->i.ds) & 0x0000FFFF;

			cu_write_reg(inst->i.ds, destination | value);
			return true;
		}
	}

	return false;
}

bool stw(CpuInstruction* inst) {
	switch(inst->r.ff) {
		case CORE32_STW_STW: {
			word source = cu_read_reg(inst->r.ra);
			word destination = cu_read_reg(inst->r.ds);

			mmu_mwrite32(mmu_tavp(destination, 'w'), source);
			return true;
		}
		case CORE32_STW_STH: {
			word source = cu_read_reg(inst->r.ra);
			word destination = cu_read_reg(inst->r.ds);

			mmu_mwrite16(mmu_tavp(destination, 'w'), source & 0xFFFF);
			return true;
		}
		case CORE32_STW_STB: {
			word source = cu_read_reg(inst->r.ra);
			word destination = cu_read_reg(inst->r.ds);

			mmu_mwrite8(mmu_tavp(destination, 'w'), source & 0xFF);
			return true;
		}
	}

	return false;
}

bool mov(CpuInstruction* inst) {
	word value = cu_read_reg(inst->r.ra);
	cu_write_reg(inst->r.ds, value);
	return true;
}

bool ja(CpuInstruction* inst) {
	inst->auto_advance = false;

	word target = cu_read_reg(inst->r.ds);
	cu_write_reg(REG_RIP, target);
	return true;
}

bool jae(CpuInstruction* inst) {
	word cmp = cu_read_reg(inst->r.ra);
	bool pass = false;

	switch(inst->r.ff) {
		case CORE32_JAE_JAE: {
			pass = (cmp & EQUAL) != 0;
			break;
		}
		case CORE32_JAE_JANE: {
			pass = (cmp & NOT_EQUAL) != 0;
			break;
		}
		case CORE32_JAE_JALT: {
			pass = (cmp & LESS_THAN) != 0;
			break;
		}
		// case CORE32_JAE_JALTU: {
		// 	break;
		// }
		case CORE32_JAE_JAGE: {
			pass = (cmp & GREATER_THAN) != 0 || (cmp & EQUAL) != 0;
			break;
		}
		// case CORE32_JAE_JAGEU: {
		// 	break;
		// }
		default: {
			return false;
		}
	}

	if(pass) {
		word target = cu_read_reg(inst->r.ds);
		cu_write_reg(REG_RIP, target);

		inst->auto_advance = false;
	} else {
		inst->auto_advance = true;
	}

	return true;
}

bool jr(CpuInstruction* inst) {
	inst->auto_advance = false;

	i32 rel = alu_signe18(inst->j.tg);
	word curr = cu_read_reg(REG_RIP);
	u32 res = curr + rel;

	cu_write_reg(REG_RIP, res);
	return true;
}

#define JR_CMP(pass) \
	word cmp = cu_read_reg(inst->j.rc); \
\
	if(pass) { \
		word current = cu_read_reg(REG_RIP); \
		word target = inst->j.tg; \
		cu_write_reg(REG_RIP, current + alu_signe18(target)); \
\
		inst->auto_advance = false; \
	} else { \
		inst->auto_advance = true; \
	} \
\
	return true;

bool jre(CpuInstruction* inst) {
	JR_CMP((cmp & EQUAL) != 0)
}

bool jrne(CpuInstruction* inst) {
	JR_CMP((cmp & NOT_EQUAL) != 0)
}

bool jrlt(CpuInstruction* inst) {
	JR_CMP((cmp & LESS_THAN) != 0)
}

bool jrge(CpuInstruction* inst) {
	JR_CMP((cmp & GREATER_THAN) != 0 || (cmp & EQUAL) != 0)
}

bool calla(CpuInstruction* inst) {
	inst->auto_advance = false;

	word current = cu_read_reg(REG_RIP);
	word target = cu_read_reg(inst->r.ds);

	cu_write_reg(REG_RTP, current + 4);
	cu_write_reg(REG_RIP, target);
	return true;
}

bool callr(CpuInstruction* inst) {
	inst->auto_advance = false;

	word current = cu_read_reg(REG_RIP);
	word target = cu_read_reg(inst->r.ds);

	cu_write_reg(REG_RTP, current + 4);
	cu_write_reg(REG_RIP, current + alu_signe18(target));
	return true;
}

bool ret(CpuInstruction* inst) {
	inst->auto_advance = false;
	cu_write_reg(REG_RIP, cu_read_reg(REG_RTP));
	return true;
}

#define CMP_BODY \
	if(a == b) { \
		result |= 1 << 0; \
	} else { \
		result |= 1 << 1; \
		if(a < b) { \
			result |= 1 << 2; \
		} \
		if(a > 0) { \
			result |= 1 << 3; \
		} \
	}

bool cmpu(CpuInstruction* inst) {
	word result = 0;

	switch(inst->r.ff) {
		case CORE32_CMPU_CMPU: {
			u32 a = cu_read_reg(inst->r.ra);
			u32 b = cu_read_reg(inst->r.rb);

			CMP_BODY
			break;
		}
		case CORE32_CMPU_CMPS: {
			i32 a = cu_read_reg(inst->r.ra);
			i32 b = cu_read_reg(inst->r.rb);

			CMP_BODY
			break;
		}
		default:
			return false;
	}

	cu_write_reg(inst->r.ds, result);
	return true;
}

bool add(CpuInstruction* inst) {
	u32 a = cu_read_reg(inst->r.ra);
	u32 b = cu_read_reg(inst->r.rb);
	u32 result = 0;

	switch(inst->r.ff) {
		case CORE32_ADD_ADD: {
			result = alu_iadd32c(a, b);
			break;
		}
		case CORE32_ADD_SUB: {
			result = alu_isub32(a, b);
			break;
		}
		default: {
			return false;
		}
	}

	cu_write_reg(inst->r.ds, result);
	return true;
}

bool addi(CpuInstruction* inst) {
	u32 a = cu_read_reg(inst->i.ds);
	u32 b = inst->i.vl;
	u32 result = alu_iadd32c(a, alu_signe16(b));

	cu_write_reg(inst->i.ds, result);
	return true;
}

bool mul(CpuInstruction* inst) {
	u32 a = cu_read_reg(inst->r.ra);
	u32 b = inst->r.rb;
	u32 result = alu_imul32(a, b);

	cu_write_reg(inst->r.ds, result);
	return true;
}

bool and(CpuInstruction* inst) {
	u32 a = cu_read_reg(inst->r.ra);
	u32 b = inst->r.rb;
	u32 result = 0;

	switch(inst->r.ff) {
		case CORE32_AND_AND: {
			result = alu_and32(a, b);
			break;
		}
		case CORE32_AND_OR: {
			result = alu_or32(a, b);
			break;
		}
		case CORE32_AND_XOR: {
			result = alu_xor32(a, b);
			break;
		}
		default: {
			return false;
		}
	}

	cu_write_reg(inst->r.ds, result);
	return true;
}

bool shl(CpuInstruction* inst) {
	u32 a = cu_read_reg(inst->r.ra);
	u32 n = inst->r.rb;
	u32 result = 0;

	switch(inst->r.ff) {
		case CORE32_SHL_SHL: {
			result = alu_lshl32(a, n);
			break;
		}
		case CORE32_SHL_SHR: {
			result = alu_lshr32(a, n);
			break;
		}
		case CORE32_SHL_SHSR: {
			result = alu_ashr32(a, n);
		}
		default:
			return false;
	}

	cu_write_reg(inst->r.ds, result);
	return true;
}

bool shli(CpuInstruction* inst) {
	u32 a = cu_read_reg(inst->i.ds);
	u32 n = inst->i.vl;
	u32 result = alu_lshl32(a, alu_zeroe16(n));

	cu_write_reg(inst->i.ds, result);
	return true;
}

bool shri(CpuInstruction* inst) {
	u32 a = cu_read_reg(inst->i.ds);
	u32 n = inst->i.vl;
	u32 result = alu_lshr32(a, alu_zeroe16(n));

	cu_write_reg(inst->i.ds, result);
	return true;
}

bool shsri(CpuInstruction* inst) {
	u32 a = cu_read_reg(inst->i.ds);
	u32 n = inst->i.vl;
	u32 result = alu_ashr32(a, alu_zeroe16(n));

	cu_write_reg(inst->i.ds, result);
	return true;
}

#define IRQ_BODY(id) \
	u8 cpl = cu_read_reg(REG_RPL);\
\
	u32 rfi0 = cu_read_reg(REG_RFI0);\
	u32 rfi1 = cu_read_reg(REG_RFI1);\
\
	switch(cpl) {\
		case PL_0: {\
			if(id >= INT_USER_MIN && id <= INT_USER_MAX) {\
				cu_raise_int(id, rfi0, rfi1);\
			} else {\
				cu_raise_int(INT_GPE, INT_GPE_0_WRONG_PRIVILEGE, INT_GPE_1(cpl, PL_1));\
			}\
			break;\
		}\
		case PL_1: {\
			cu_raise_int(id, rfi0, rfi1);\
			break;\
		}\
	}

bool int_(CpuInstruction* inst) {
	u8 id = cu_read_reg(inst->r.ds);
	IRQ_BODY(id);

	return true;
}

bool inti(CpuInstruction* inst) {
	u8 id = cu_read_reg(inst->i.vl);
	IRQ_BODY(id);

	return true;
}

bool intm(CpuInstruction* inst) {
	u8 cpl = cu_read_reg(REG_RPL);

	if(cpl != PL_1) {
		cu_raise_int(INT_GPE, INT_GPE_0_WRONG_PRIVILEGE, INT_GPE_1(cpl, PL_1));
		return true;
	}
	
	u8 id = cu_read_reg(inst->i.ds);
	bool enable = inst->i.vl;

	cu_enable_irh(id, enable);
	return true;
}

bool reti(CpuInstruction* inst) {
	inst->auto_advance = false;
	cu_write_reg(REG_RFP, cu_read_reg(REG_RLFP));
	cu_write_reg(REG_RIP, cu_read_reg(REG_RIP) + 4);
	return true;
}

bool mcw(CpuInstruction* inst) {
	word destination = cu_read_reg(inst->r.ds);
	word source = cu_read_reg(inst->r.ra);

	switch(inst->r.ff) {
		case CORE32_MCW_MCW: {
			mmu_mwrite32(mmu_tavp(destination, 'w'), mmu_mread32(mmu_tavp(source, 'r')));
			break;
		}
		case CORE32_MCW_MCH: {
			mmu_mwrite16(mmu_tavp(destination, 'w'), mmu_mread16(mmu_tavp(source, 'r')));
			break;
		}
		case CORE32_MCW_MCB: {
			mmu_mwrite8(mmu_tavp(destination, 'w'), mmu_mread8(mmu_tavp(source, 'r')));
			break;
		}
		default:
			return false;
	}

	return true;
}

bool undefined(CpuInstruction* inst) {
	return false;
}

OpSetEntry core32_opset[OPSET_SIZE] = {
	{ REGISTER, nop },
	{ REGISTER, cpuid },
	{ REGISTER, ldw },
	{ IMMEDIATE, lvi },
	{ IMMEDIATE, lli },
	{ REGISTER, stw },
	{ REGISTER, mov },
	{ REGISTER, ja },
	{ REGISTER, jae },
	{ JUMP, jr },
	{ JUMP, jre },
	{ JUMP, jrne },
	{ JUMP, jrlt },
	{ UNKNOWN, undefined },
	{ JUMP, jrge },
	{ UNKNOWN, undefined },
	{ REGISTER, calla },
	{ JUMP, callr },
	{ REGISTER, ret },
	{ REGISTER, cmpu },
	{ REGISTER, add },
	{ IMMEDIATE, addi },
	{ REGISTER, mul },
	{ REGISTER, and },
	{ REGISTER, shl },
	{ IMMEDIATE, shli },
	{ REGISTER, int_ },
	{ JUMP, inti },
	{ IMMEDIATE, intm },
	{ REGISTER, reti },
	{ REGISTER, mcw },
	{ UNKNOWN, undefined }
};
