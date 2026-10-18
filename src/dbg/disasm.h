#pragma once
#include "arch_types.h"
#include "isa/op.h"

#define MAX_REGISTERS 0x20
#define MAX_INSTRUCTIONS 0xFF

typedef struct DebugRegisterTableEntry {
	const char* name;
} DebugRegisterTableEntry;

extern DebugRegisterTableEntry dbg_registers[MAX_REGISTERS];

typedef struct DebugInstructionTableEntry {
	u8 os;
	u8 op;
	u8 variant;
	const char* name;
	bool use_param0;
	bool use_param1;
	bool use_param2;
} DebugInstructionTableEntry;

extern DebugInstructionTableEntry dbg_instructions[MAX_INSTRUCTIONS];

typedef struct DebugDasmInstruction {
	char* name;
	char* param0;
	char* param1;
	char* param2;
	CpuInstruction inst;
} DebugDasmInstruction;

void dbg_disasm_init(DebugDasmInstruction* dasm);
void dbg_disasm_free(DebugDasmInstruction* dasm);

bool dbg_disasm_inst(DebugDasmInstruction* dasm, word data);
