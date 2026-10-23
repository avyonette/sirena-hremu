#pragma once
#include <types.h>
#include <arch_types.h>
#include "reg.h"
#include "int.h"

#define CLK_MAIN_FREQ 100000

#define PL_0 0x00
#define PL_1 0x01

#define PL_CHECK(req) \
	if(cu_read_reg(REG_RPL) != PL_1) { \
		cu_raise_int(INT_GPE, INT_GPE_0_WRONG_PRIVILEGE, INT_GPE_1(cu_read_reg(REG_RPL), PL_1)); \
		return; \
	}
#define PL_CHECK_RET(req, ret) \
	if(cu_read_reg(REG_RPL) != PL_1) { \
		cu_raise_int(INT_GPE, INT_GPE_0_WRONG_PRIVILEGE, INT_GPE_1(cu_read_reg(REG_RPL), PL_1)); \
		return ret; \
	}

extern bool irq_map[INT_MAX + 1];

void cu_init();
void cu_reset();
void cu_step();
void cu_clock();

void cu_pause();
void cu_resume();

void cu_raise_int(u8 id, word rfi0, word rfi1);
void cu_enable_irh(u8 id, bool enable);

word cu_read_reg(u8 idx);
void cu_write_reg(u8 idx, word value);
