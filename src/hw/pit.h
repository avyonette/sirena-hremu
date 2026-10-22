#pragma once
#include <time.h>
#include "types.h"
#include "arch_types.h"

#define PIT_CMD_SET_MODE 0x00
#define PIT_CMD_SET_TICK_RATE 0x01
#define PIT_CMD_SET_TARGET_INT 0x02
#define PIT_CMD_GET_TICK_COUNT 0x03
#define PIT_CMD_RESET_COUNTER 0x04

typedef enum PIT_Mode {
	PIT_DISABLED,
	PIT_ONE_SHOT,
	PIT_TICK,
} PIT_Mode;

typedef struct PIT_CommandFrame {
	u8 cmd : 3;
	u32 dt : 29;
} PIT_CommandFrame;

typedef struct PIT {
	u32 counter;
	PIT_Mode mode;
	u32 tick_rate; // in Hz
	u8 target_int;

	// internal emulator "hardware" implementation specifics
	struct timespec last_tick;
} PIT;

#define PIT_COUNT 4
extern PIT pit_timers[PIT_COUNT];

void pit_tick_0();
void pit_tick_1();
void pit_tick_2();
void pit_tick_3();

word pit_io_read(PIT* pit, p_addr address);
void pit_io_write(PIT* pit, p_addr address, word data);
