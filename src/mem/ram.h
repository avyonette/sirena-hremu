#pragma once
#include "mem.h"

// in bytes
#define USER_RAM_SIZE 64 * 1024 * 1024
#define SYSTEM_RAM_SIZE 160 * 1024

extern HardwareMemory user_ram;
extern HardwareMemory system_ram;
