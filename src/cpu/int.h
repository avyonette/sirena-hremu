#pragma once
#include <arch_types.h>
#include <types.h>

#define INT_MAX 0xFF
#define INT_USER_MIN 0x20
#define INT_USER_MAX INT_MAX

#define INT_UDO 0x01
#define INT_DBG 0x02
#define INT_BRK 0x03
#define INT_IOP 0x04
#define INT_DBZ 0x05
#define INT_PLE 0x06
#define INT_MAE 0x07
#define INT_GPE 0x08

#define INT_IOP_0_WRITE_TO_RO_REG 0x00
#define INT_IOP_0_FP_OUTSIDE_SYSTEM 0x01
#define INT_IOP_0_UNKNOWN_FEATURE 0x02
#define INT_IOP_0_OUT_OF_RANGE 0x03
#define INT_IOP_0_WRONG_DATA_SIZE 0x04
#define INT_IOP_0_OTHER 0xFF

#define INT_UDO_0(op, os) (os & 0b1111) | ((op & 0b11111) << 4)

#define INT_PLE_1_NOT_FOUND 0x00

#define INT_MAE_1_UNKNOWN_READ 0x00
#define INT_MAE_1_UNKNOWN_WRITE 0x01
#define INT_MAE_1_WRONG_ALIGNMENT 0x02
#define INT_MAE_1_INVALID_ADDRESS 0x03
#define INT_MAE_1_WRITE_TO_ROM 0x04

#define INT_GPE_0_WRONG_PRIVILEGE 0x00
#define INT_GPE_0_EXEC_ON_NO_EXEC 0x01
#define INT_GPE_0_WRITE_ON_READ_ONLY 0x02
#define INT_GPE_0_UNDEFINED_PRIVILEGE 0x03

#define INT_GPE_1(cpl, npl) (cpl & 0b11) | ((npl & 0b11) << 2)

#pragma pack(push, 1)
typedef struct InterruptRoutineArrayEntry {
	bool en : 1;
	u32 _reserved_0 : 31;
	p_addr rfp;
	p_addr rh;
} InterruptRoutineArrayEntry;

typedef struct InterruptRoutineArray {
	InterruptRoutineArrayEntry entries[INT_MAX + 1];
} InterruptRoutineArray;
#pragma pack(pop)
