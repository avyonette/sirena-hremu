#pragma once
#include "arch_types.h"
#include "op.h"

u8 isa_read_os(word data);
u8 isa_read_op(word data);

bool isa_decode(CpuInstruction* inst, word data);
bool isa_execute(CpuInstruction* inst);
