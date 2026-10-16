#pragma once
#include "mem.hpp"

// in bytes
#define USER_RAM_SIZE 64 * 1024 * 1024
#define SYSTEM_RAM_SIZE 160 * 1024

namespace hremu::mem {
	extern ReadWriteMemory user_ram;
	extern ReadWriteMemory system_ram;
}
