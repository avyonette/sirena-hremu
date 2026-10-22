#include "disasm.h"
#include "cpu/alu.h"
#include "cpu/reg.h"
#include "isa/core32.h"
#include "isa/isa.h"
#include <stdio.h>
#include <stdlib.h>

DebugRegisterTableEntry dbg_registers[MAX_REGISTERS] = {
	{ "r0" },
	{ "r1" },
	{ "r2" },
	{ "r3" },
	{ "r4" },
	{ "r5" },
	{ "r6" },
	{ "r7" },
	{ "r8" },
	{ "r9" },
	{ "r10" },
	{ "r11" },
	{ "r12" },
	{ "r13" },
	{ "r14" },
	{ "r15" },
	{ "rip" },
	{ "rtp" },
	{ "rsp" },
	{ "rlfp" },
	{ "rpfp" },
	{ "rcf" },
	{ "rpl" },
	{ "??" },
	{ "rfi0" },
	{ "rfi1" },
	{ "rfo0" },
	{ "rfo1" },
	{ "??" },
	{ "rfp" },
	{ "rrap" },
	{ "rz" }
};

#define MAX_INSTRUCTION_NAME_LENGTH 16
#define MAX_INSTRUCTION_PARAM_LENGTH 16

DebugInstructionTableEntry dbg_instructions[MAX_INSTRUCTIONS] = {
	{ CORE32_OPSET, CORE32_NOP, 0, "nop", false, false, false },
	{ CORE32_OPSET, CORE32_CPUID, 0, "cpuid", true, false, false },
	{ CORE32_OPSET, CORE32_LDW, CORE32_LDW_LDW, "ldw", true, true, false },
	{ CORE32_OPSET, CORE32_LDW, CORE32_LDW_LDHZ, "ldhz", true, true, false },
	{ CORE32_OPSET, CORE32_LDW, CORE32_LDW_LDHS, "ldhs", true, true, false },
	{ CORE32_OPSET, CORE32_LDW, CORE32_LDW_LDBZ, "ldbz", true, true, false },
	{ CORE32_OPSET, CORE32_LDW, CORE32_LDW_LDBS, "ldbs", true, true, false },
	{ CORE32_OPSET, CORE32_LVI, 0, "lvi", true, true, false },
	{ CORE32_OPSET, CORE32_LLI, CORE32_LLI_LLI, "lli", true, true, false },
	{ CORE32_OPSET, CORE32_LLI, CORE32_LLI_LUI, "lui", true, true, false },
	{ CORE32_OPSET, CORE32_STW, CORE32_STW_STW, "stw", true, true, false },
	{ CORE32_OPSET, CORE32_STW, CORE32_STW_STH, "sth", true, true, false },
	{ CORE32_OPSET, CORE32_STW, CORE32_STW_STB, "stb", true, true, false },
	{ CORE32_OPSET, CORE32_MOV, 0, "mov", true, true, false },
	{ CORE32_OPSET, CORE32_JA, 0, "ja", true, false, false },
	{ CORE32_OPSET, CORE32_JAE, CORE32_JAE_JAE, "jae", true, true, false },
	{ CORE32_OPSET, CORE32_JAE, CORE32_JAE_JANE, "jane", true, true, false },
	{ CORE32_OPSET, CORE32_JAE, CORE32_JAE_JALT, "jalt", true, true, false },
	{ CORE32_OPSET, CORE32_JAE, CORE32_JAE_JAGE, "jage", true, true, false },
	{ CORE32_OPSET, CORE32_JR, 0, "jr", true, false, false },
	{ CORE32_OPSET, CORE32_JRE, 0, "jre", true, true, false },
	{ CORE32_OPSET, CORE32_JRNE, 0, "jrne", true, true, false },
	{ CORE32_OPSET, CORE32_JRGE, 0, "jrge", true, true, false },
	{ CORE32_OPSET, CORE32_CALLA, 0, "calla", true, false, false },
	{ CORE32_OPSET, CORE32_CALLR, 0, "callr", true, false, false },
	{ CORE32_OPSET, CORE32_RET, 0, "ret", false, false, false },
	{ CORE32_OPSET, CORE32_CMPU, CORE32_CMPU_CMPU, "cmpu", true, true, true },
	{ CORE32_OPSET, CORE32_CMPU, CORE32_CMPU_CMPS, "cmps", true, true, true },
	{ CORE32_OPSET, CORE32_ADD, CORE32_ADD_ADD, "add", true, true, true },
	{ CORE32_OPSET, CORE32_ADD, CORE32_ADD_SUB, "sub", true, true, true },
	{ CORE32_OPSET, CORE32_ADDI, 0, "addi", true, true, false },
	{ CORE32_OPSET, CORE32_MUL, 0, "mul", true, true, true },
	{ CORE32_OPSET, CORE32_AND, CORE32_AND_AND, "and", true, true, true },
	{ CORE32_OPSET, CORE32_AND, CORE32_AND_OR, "or", true, true, true },
	{ CORE32_OPSET, CORE32_AND, CORE32_AND_XOR, "xor", true, true, true },
	{ CORE32_OPSET, CORE32_SHL, CORE32_SHL_SHL, "shl", true, true, true },
	{ CORE32_OPSET, CORE32_SHL, CORE32_SHL_SHR, "shr", true, true, true },
	{ CORE32_OPSET, CORE32_SHL, CORE32_SHL_SHSR, "shsr", true, true, true },
	{ CORE32_OPSET, CORE32_SHLI, CORE32_SHLI_SHLI, "shli", true, true, false },
	{ CORE32_OPSET, CORE32_SHLI, CORE32_SHLI_SHRI, "shri", true, true, false },
	{ CORE32_OPSET, CORE32_SHLI, CORE32_SHLI_SHSRI, "shsri", true, true, false },
	{ CORE32_OPSET, CORE32_INT, 0, "int", true, false, false },
	{ CORE32_OPSET, CORE32_INTI, 0, "inti", false, true, false },
	{ CORE32_OPSET, CORE32_INTM, 0, "intm", true, true, false },
	{ CORE32_OPSET, CORE32_RETI, 0, "reti", false, false, false },
	{ CORE32_OPSET, CORE32_MCW, CORE32_MCW_MCW, "mcw", true, true, false },
	{ CORE32_OPSET, CORE32_MCW, CORE32_MCW_MCH, "mch", true, true, false },
	{ CORE32_OPSET, CORE32_MCW, CORE32_MCW_MCB, "mcb", true, true, false },
	{ 0, 0, 0, 0 }
};

void dbg_disasm_init(DebugDasmInstruction* dasm) {
	dasm->name = calloc(MAX_INSTRUCTION_NAME_LENGTH + 1, sizeof(char));
	dasm->param0 = calloc(MAX_INSTRUCTION_PARAM_LENGTH + 1, sizeof(char));
	dasm->param1 = calloc(MAX_INSTRUCTION_PARAM_LENGTH + 1, sizeof(char));
	dasm->param2 = calloc(MAX_INSTRUCTION_PARAM_LENGTH + 1, sizeof(char));
}

void dbg_disasm_free(DebugDasmInstruction* dasm) {
	free(dasm->name);
	free(dasm->param0);
	free(dasm->param1);
	free(dasm->param2);
}

bool dbg_disasm_inst(DebugDasmInstruction* dasm, word data) {
	assert(dasm);
	assert(dasm->name);
	assert(dasm->param0);
	assert(dasm->param1);
	assert(dasm->param2);

	if(!isa_decode(&dasm->inst, data)) {
		return false;
	}

	DebugInstructionTableEntry dbg_inst = {0};

	for(usize i = 0; i < MAX_INSTRUCTIONS; i++) {
		dbg_inst = dbg_instructions[i];

		if(!dbg_inst.name) {
			// instruction not in table
			sprintf(dasm->name, "??");
			break;
		}

		// decode instruction name
		if(dbg_inst.os == dasm->inst.os && dbg_inst.op == dasm->inst.op) {
			switch(dasm->inst.type) {
				case REGISTER: {
					if(dbg_inst.variant == dasm->inst.r.ff) {
						sprintf(dasm->name, "%s", dbg_inst.name);
						goto found_name;
					}
					break;
				}
				case IMMEDIATE: {
					if(dbg_inst.variant == dasm->inst.i.ff) {
						sprintf(dasm->name, "%s", dbg_inst.name);
						goto found_name;
					}
					break;
				}
				case JUMP: {
					sprintf(dasm->name, "%s", dbg_inst.name);
					goto found_name;
				}
				default:
					sprintf(dasm->name, "%#04x:%#04x", dasm->inst.os, dasm->inst.op);
			}
		}
	}

	if(!dbg_inst.name) {
		return false;
	}

found_name: ;

	// decode instruction params
	switch(dasm->inst.type) {
		case REGISTER: {
			// ds
			if(dbg_inst.use_param0) {
				if(dasm->inst.r.ds > MAX_INSTRUCTIONS - 1) sprintf(dasm->param0, "??");
				else sprintf(dasm->param0, "%s", dbg_registers[dasm->inst.r.ds].name);
			}

			// ra
			if(dbg_inst.use_param1) {
				if(dasm->inst.r.ra > MAX_INSTRUCTIONS - 1) sprintf(dasm->param1, "??");
				else sprintf(dasm->param1, "%s", dbg_registers[dasm->inst.r.ra].name);
			}

			// rb
			if(dbg_inst.use_param2) {
				if(dasm->inst.r.rb > MAX_INSTRUCTIONS - 1) sprintf(dasm->param2, "??");
				else sprintf(dasm->param2, "%s", dbg_registers[dasm->inst.r.rb].name);
			}
			break;
		}
		case IMMEDIATE: {
			// ds
			if(dbg_inst.use_param0) {
				if(dasm->inst.i.ds > MAX_INSTRUCTIONS - 1) sprintf(dasm->param0, "??");
				else sprintf(dasm->param0, "%s", dbg_registers[dasm->inst.i.ds].name);
			}

			// vl
			if(dbg_inst.use_param1) {
				sprintf(dasm->param1, "%#x", dasm->inst.i.vl);
			}
			break;
		}
		case JUMP: {
			// tg
			if(dbg_inst.use_param0) {
				sprintf(dasm->param0, "%d", alu_signe18(dasm->inst.j.tg)); // for debugging it can be assumed that tg in J-type is always zero-extended
			}

			// rc
			if(dbg_inst.use_param1) {
				if(dasm->inst.j.rc > MAX_INSTRUCTIONS - 1) sprintf(dasm->param1, "??");
				else sprintf(dasm->param1, "%s", dbg_registers[dasm->inst.j.rc].name);
			}
			break;
		}
		default:
			sprintf(dasm->param0, "??");
			sprintf(dasm->param1, "??");
			sprintf(dasm->param2, "??");
	}

	return true;
}
