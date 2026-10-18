#pragma once
#include "types.h"

extern struct arg_file* cli_blip;
extern struct arg_lit* cli_auto_dump_regs;
extern struct arg_lit* cli_auto_disasm;
extern struct arg_lit* cli_headless;
extern struct arg_end* cli_end;

extern u64 cycle_count;

extern bool opt_dump_regs_after_cycle;
extern bool opt_auto_disassemble;
extern bool opt_headless;
