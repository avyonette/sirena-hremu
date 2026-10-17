#include "core32.h"
#include "cpu/alu.h"
#include "cpu/cu.h"

bool nop(CpuInstruction* inst) {
	return true;
}

bool jr(CpuInstruction* inst) {
	i32 rel = sext17(inst->j.tg);
	word curr = cu_read_reg(REG_RIP);
	u32 res = curr + rel;

	cu_write_reg(REG_RIP, res);
	return true;
}

bool undefined(CpuInstruction* inst) {
	cu_raise_int(INT_UDO, INT_UDO_0(inst->op, inst->os), 0);
	return false;
}

OpSetEntry core32_opset[OPSET_SIZE] = {
	{ REGISTER, nop, true },
	{ REGISTER, undefined, true },
	{ REGISTER, undefined, true },
	{ IMMEDIATE, undefined, true },
	{ IMMEDIATE, undefined, true },
	{ REGISTER, undefined, true },
	{ REGISTER, undefined, true },
	{ REGISTER, undefined, true },
	{ REGISTER, undefined, true },
	{ JUMP, jr, false }, // jump/call instructions do not advance rip automatically
	{ JUMP, undefined, false },
	{ JUMP, undefined, false },
	{ JUMP, undefined, false },
	{ JUMP, undefined, false },
	{ JUMP, undefined, false },
	{ JUMP, undefined, false },
	{ REGISTER, undefined, false },
	{ JUMP, undefined, false },
	{ REGISTER, undefined, false },
	{ REGISTER, undefined, true },
	{ REGISTER, undefined, true },
	{ IMMEDIATE, undefined, true },
	{ REGISTER, undefined, true },
	{ REGISTER, undefined, true },
	{ REGISTER, undefined, true },
	{ IMMEDIATE, undefined, true },
	{ REGISTER, undefined, true },
	{ IMMEDIATE, undefined, true },
	{ REGISTER, undefined, true },
	{ REGISTER, undefined, true },
	{ REGISTER, undefined, true },
	{ REGISTER, undefined, true }
};
