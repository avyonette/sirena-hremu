// simulates a crystal oscillator
#pragma once
#include "types.h"

typedef void (*ClockCallbackFn)();

void clk_create(usize id, usize frequency_hz, ClockCallbackFn callback, bool initial_state);
void clk_destroy(usize id);

void clk_pause(usize id);
void clk_resume(usize id);
