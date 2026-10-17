#pragma once
#include "op.h"

#define CORE32_OPSET 0x00

#define CORE32_NOP 0x00
#define CORE32_CPUID 0x01

#define CORE32_LDW 0x02
#define CORE32_LDW_LDW 0x00
#define CORE32_LDW_LDHZ 0x01
#define CORE32_LDW_LDHS 0x02
#define CORE32_LDW_LDBZ 0x03
#define CORE32_LDW_LDBS 0x04
#define CORE32_LVI 0x03
#define CORE32_LLI 0x04
#define CORE32_LLI_LLI 0x00
#define CORE32_LLI_LUI 0x01
#define CORE32_STW 0x05
#define CORE32_STW_STW 0x00
#define CORE32_STW_STH 0x01
#define CORE32_STW_STB 0x02

#define CORE32_MOV 0x06

#define CORE32_JA 0x07
#define CORE32_JAE 0x08
#define CORE32_JAE_JAE 0x00
#define CORE32_JAE_JANE 0x01
#define CORE32_JAE_JALT 0x02
//#define CORE32_JAE_JALTU 0x03
#define CORE32_JAE_JAGE 0x04
//#define CORE32_JAE_JAGEU 0x05
#define CORE32_JR 0x09
#define CORE32_JRE 0x0A
#define CORE32_JRNE 0x0B
#define CORE32_JRLT 0x0C
//#define CORE32_JRLTU 0x0D
#define CORE32_JRGE 0x0E
//#define CORE32_JRGEU 0x0F
#define CORE32_CALLA 0x10
#define CORE32_CALLR 0x11
#define CORE32_RET 0x12

#define CORE32_CMPU 0x13
#define CORE32_CMPU_CMPU 0x00
#define CORE32_CMPU_CMPS 0x01
#define CORE32_ADD 0x14
#define CORE32_ADD_ADD 0x00
#define CORE32_ADD_SUB 0x01
#define CORE32_ADDI 0x15
#define CORE32_MUL 0x16

#define CORE32_AND 0x17
#define CORE32_AND_AND 0x00
#define CORE32_AND_OR 0x01
#define CORE32_AND_XOR 0x02
#define CORE32_SHL 0x18
#define CORE32_SHL_SHL 0x00
#define CORE32_SHL_SHR 0x01
#define CORE32_SHL_SHSR 0x02
#define CORE32_SHLI 0x19
#define CORE32_SHLI_SHLI 0x00
#define CORE32_SHLI_SHRI 0x01
#define CORE32_SHLI_SHSRI 0x02

#define CORE32_INT 0x1A
#define CORE32_INTI 0x1B
#define CORE32_INTM 0x1C
#define CORE32_RETI 0x1D

#define CORE32_MCW 0x1E
#define CORE32_MCW_MCW 0x00
#define CORE32_MCW_MCH 0x01
#define CORE32_MCW_MCB 0x02

typedef enum CompareResultBitmask {
	EQUAL = 1 << 0,
	NOT_EQUAL = 1 << 1,
	LESS_THAN = 1 << 2,
	GREATER_THAN = 1 << 3
} CompareResultBitmask;

extern OpSetEntry core32_opset[OPSET_SIZE];
