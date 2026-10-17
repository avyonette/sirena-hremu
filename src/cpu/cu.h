#pragma once
#include <types.h>
#include <arch_types.h>
#include "reg.h"
#include "int.h"

extern bool irq_map[INT_MAX + 1];

// these registers live inside the CPU so it's fine to have them be pointers to real host structs
extern RegisterFrame* rfp;
extern InterruptRoutineArray* rrap;
extern word rz;

void cu_init();
void cu_step();

void cu_raise_int(u8 id, word rfi0, word rfi1);
