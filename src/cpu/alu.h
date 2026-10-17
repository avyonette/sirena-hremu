#pragma once
#include "types.h"

u32 alu_iadd32c(u32 a, u32 b);
u32 alu_isub32(u32 a, u32 b);
u32 alu_imul32(u32 a, u32 b);

u32 alu_and32(u32 a, u32 b);
u32 alu_or32(u32 a, u32 b);
u32 alu_xor32(u32 a, u32 b);

u32 alu_lshl32(u32 a, u32 n);
u32 alu_lshr32(u32 a, u32 n);
u32 alu_ashr32(u32 a, u32 n);

u32 alu_signe18(u32 a);
u32 alu_signe16(u32 a);
u32 alu_signe8(u32 a);
u32 alu_zeroe16(u32 a);
u32 alu_zeroe8(u32 a);
