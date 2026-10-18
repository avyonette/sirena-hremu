#pragma once
#include "arch_types.h"
#include <stdio.h>

void dbg_cpu_dump_regs(FILE* out);
void dbg_cpu_get_rip_data32(word* data);
