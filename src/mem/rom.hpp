#pragma once
#include "mem.hpp"

// in bytes
#define BLIP_ROM_SIZE 64 * 1024

namespace hremu::mem {
	extern ReadOnlyMemory blip_rom;
}
