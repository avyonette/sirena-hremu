#include <pthread.h>
#include "clk.h"

typedef struct ClockTickerArgs {
	usize id;
	bool state;
	bool paused;
	u64 interval_ns;
	ClockCallbackFn callback;
} ClockTickerArgs;

#define MAX_CLOCKS 64
pthread_t clk_threads[MAX_CLOCKS] = {0};
ClockTickerArgs clk_args[MAX_CLOCKS] = {0};

// TODO this intervaling shit sucks and never works because of inaccuracies or some shit (pit.c as well)
// it can't even be called "good enough" because it won't work for shit that needs a steady clock like UART
void* clk_ticker(void* ptr) {
	usize id = (usize) ptr;
	ClockTickerArgs* args = &clk_args[id];

	struct timespec t_last = {0};

	while(args->state) {
		if(args->paused) continue;

		struct timespec t_now = {0};
		clock_gettime(CLOCK_MONOTONIC, &t_now);

		if(t_now.tv_nsec - t_last.tv_nsec >= args->interval_ns) {
			args->callback();
		}

		clock_gettime(CLOCK_MONOTONIC, &t_last);
	}

	return NULL;
}

void clk_create(usize id, usize frequency_hz, ClockCallbackFn callback, bool initial_state) {
	if(clk_threads[id] != 0) {
		clk_destroy(id);
	}

	ClockTickerArgs args = {
		.id = id,
		.state = true,
		.paused = initial_state,
		.interval_ns = 1e9 / frequency_hz,
		.callback = callback
	};
	clk_args[id] = args;

	pthread_t clock_thread;
	pthread_create(&clock_thread, NULL, clk_ticker, (void*) id);

	clk_threads[id] = clock_thread;
}

void clk_destroy(usize id) {
	clk_args[id].state = false;
	pthread_cancel(clk_threads[id]);
	clk_threads[id] = 0;
}

void clk_pause(usize id) {
	clk_args[id].paused = true;
}

void clk_resume(usize id) {
	clk_args[id].paused = false;
}
