#include "pit.h"
#include "cpu/cu.h"
#include "cpu/int.h"
#include "log.h"

PIT pit_timers[PIT_COUNT] = {0};

void pit_tick_single(PIT* pit) {
	if(pit->mode == PIT_DISABLED) return;

	struct timespec t_now = {0};
	clock_gettime(CLOCK_MONOTONIC, &t_now);

	if(!(t_now.tv_nsec - pit->last_tick.tv_nsec >= 1e9 / pit->tick_rate)) {
		return;
	}

	clock_gettime(CLOCK_MONOTONIC, &pit->last_tick);
	EMU_DBG("PIT %p called", pit);
	
	switch(pit->mode) {
		case PIT_ONE_SHOT: {
			if(pit->target_int > 0) {
				cu_raise_int(pit->target_int, 0, 0);
			}

			pit->mode = PIT_DISABLED;
			pit->counter++;
			break;
		}
		case PIT_TICK: {
			if(pit->target_int > 0) {
				cu_raise_int(pit->target_int, 0, 0);
			}

			pit->counter++;
			break;
		}
		default:
			assert(false);
			break;
	}
}

void pit_tick_0() { pit_tick_single(&pit_timers[0]); }
void pit_tick_1() { pit_tick_single(&pit_timers[1]); }
void pit_tick_2() { pit_tick_single(&pit_timers[2]); }
void pit_tick_3() { pit_tick_single(&pit_timers[3]); }

word pit_io_read(PIT* pit, p_addr address) {
	return 0;
}

void pit_io_write(PIT* pit, p_addr address, word data) {
	if(address == 0) { // command
		PIT_CommandFrame* cmd_frame = (PIT_CommandFrame*) &data;

		switch(cmd_frame->cmd) {
			case PIT_CMD_SET_MODE: {
				if(cmd_frame->dt > PIT_TICK) {
					cu_raise_int(INT_IOP, INT_IOP_0_OUT_OF_RANGE, 0);
				} else {
					pit->mode = cmd_frame->dt;
				}

				break;
			}
			case PIT_CMD_SET_TICK_RATE: {
				pit->tick_rate = cmd_frame->dt;
				break;
			}
			case PIT_CMD_SET_TARGET_INT: {
				pit->target_int = cmd_frame->dt;
				break;
			}
			case PIT_CMD_GET_TICK_COUNT: {
				cu_write_reg(cmd_frame->dt, pit->counter);
				break;
			}
			case PIT_CMD_RESET_COUNTER: {
				pit->counter = 0;
				break;
			}
			default:
				cu_raise_int(INT_UDO, INT_UDO_0(cmd_frame->cmd, 0), 0);
		}
	}
}
