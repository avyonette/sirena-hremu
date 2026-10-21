#pragma once
#include "types.h"

#define INSTRUCTION_SIZE 4

#pragma pack(push, 1)
typedef struct RInstruction {
	u8 os : 4;
	u8 op : 5;
	u8 ds : 5;
	u8 ra : 5;
	u8 rb : 5;
	u8 ff : 8;
} RInstruction;

typedef struct IInstruction {
	u8 os : 4;
	u8 op : 5;
	u8 ds : 5;
	u16 vl : 16;
	u8 ff : 2;
} IInstruction;

typedef struct JInstruction {
	u8 os : 4;
	u8 op : 5;
	u32 tg : 18;
	u8 rc : 5;
} JInstruction;
#pragma pack(pop)

typedef enum CpuInstructionType {
	UNKNOWN,
	REGISTER,
	IMMEDIATE,
	JUMP,
} CpuInstructionType;

typedef struct CpuInstruction {
	CpuInstructionType type;

	u8 os;
	u8 op;

	bool auto_advance;

	union {
		RInstruction r;
		IInstruction i;
		JInstruction j;
	};
} CpuInstruction;

#define OPSET_SIZE 32

typedef bool (*OpExecute)(CpuInstruction* inst);

typedef struct OpSetEntry {
	CpuInstructionType type;
	OpExecute exec;
} OpSetEntry;
