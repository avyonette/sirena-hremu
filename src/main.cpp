#include <spdlog/spdlog.h>
#include "mem/ram.hpp"
#include "mem/rom.hpp"

using namespace hremu;

int main() {
	spdlog::info("User RAM size: {}", mem::user_ram.size);
	spdlog::info("System RAM size: {}", mem::system_ram.size);
	spdlog::info("BLIP ROM size: {}", mem::blip_rom.size);
	return 0;
}
