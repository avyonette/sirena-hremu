#include "ram.hpp"

namespace hremu::mem {
	ReadWriteMemory user_ram(USER_RAM_SIZE);
	ReadWriteMemory system_ram(SYSTEM_RAM_SIZE);
}
