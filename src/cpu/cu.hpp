#pragma once
#include <types.h>
#include <arch_types.h>
#include "reg.hpp"
#include "int.hpp"

namespace hremu::cpu {
	extern bool irq_map[INT_MAX + 1];

	// these registers live inside the CPU so it's fine to have them be pointers to real host structs
	extern RegisterFrame* rfp;
	extern InterruptRoutineArray* rrap;
	extern word rz;

	void init();
	void step();

	void raise_int(u8 id, word rfi0, word rfi1);
}
