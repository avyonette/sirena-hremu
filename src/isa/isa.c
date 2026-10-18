#include "isa.h"
#include "core32.h"
#include "cpu/cu.h"

u8 isa_read_os(word data) {
	return data & 0b1111;
}

u8 isa_read_op(word data) {
	return (data >> 4) & 0b11111;
}

bool isa_decode(CpuInstruction* inst, word data) {
	assert(inst);
	
	u8 os = isa_read_os(data);
	u8 op = isa_read_op(data);

	inst->os = os;
	inst->op = op;

	switch(os) {
		case CORE32_OPSET: {
			inst->type = core32_opset[op].type;
			break;
		}
		default: {
			return false;
		}
	}

	switch(inst->type) {
		case REGISTER: {
			inst->r.op = op;
			inst->r.os = os;
			inst->r.ds = (data >> 9) & 0b11111;
			inst->r.ra = (data >> 14) & 0b11111;
			inst->r.rb = (data >> 19) & 0b11111;
			inst->r.ff = (data >> 24) & 0b11111111;
			break;
		};
		case IMMEDIATE: {
			inst->i.op = op;
			inst->i.os = os;
			inst->i.ds = (data >> 9) & 0b11111;
			inst->i.vl = (data >> 14) & 0b1111111111111111;
			inst->i.ff = (data >> 30) & 0b11;
			break;
		};
		case JUMP: {
			inst->i.op = op;
			inst->i.os = os;
			inst->j.tg = (data >> 9) & 0b111111111111111111;
			inst->j.rc = (data >> 27) & 0b11111;
			break;
		}
		default: {
			return false;
		};
	}

	return true;
}

bool isa_execute(CpuInstruction* inst) {
	inst->auto_advance = true; // default advance (7. "the CPU must advance the (rip) by a single word for any instruction that does not explicitly set the (rip)")

	switch(inst->os) {
		case CORE32_OPSET: {
			if(!core32_opset[inst->op].exec(inst)) {
				inst->auto_advance = true; // restore advance
				return false;
			}
			break;
		}
		default:
			return false;
	}

	return true;
}
